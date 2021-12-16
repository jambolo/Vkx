#include "Image.h"

#include "Buffer.h"
#include "Vkx.h"

#include <vulkan/vulkan.hpp>

#include <array>
#include <cmath>

namespace Vkx
{
//! @param  device              Logical device associated with the image
//! @param  info                Creation info
//! @param  memoryProperties    Memory properties
//! @param  aspect
//!
//! @warning       A std::runtime_error is thrown if the image cannot be created and allocated
//! @todo       "... you're not supposed to actually call vkAllocateMemory for every individual image. ... The right way to
//!             allocate memory for a large number of objects at the same time is to create a custom allocator that splits up a
//!             single allocation among many different objects by using the offset parameters that we've seen in many functions."

Image::Image(std::shared_ptr<Device>     device,
             vk::ImageCreateInfo const & info,
             vk::MemoryPropertyFlags     memoryProperties,
             vk::ImageAspectFlags        aspect)
    : device_(device)
    , info_(info)
{
    image_ = device->createImageUnique(info_);

    vk::MemoryRequirements requirements = device->getImageMemoryRequirements(*image_);
    uint32_t memoryType = findAppropriateMemoryType(device->physical(), requirements.memoryTypeBits, memoryProperties);

    allocation_ = device->allocateMemoryUnique(vk::MemoryAllocateInfo(requirements.size, memoryType));
    device->bindImageMemory(*image_, *allocation_, 0);

    view_ = device->createImageViewUnique(
        vk::ImageViewCreateInfo({},
                                *image_,
                                vk::ImageViewType::e2D,
                                info_.format,
                                vk::ComponentMapping(),
                                vk::ImageSubresourceRange(aspect, 0, info_.mipLevels, 0, 1)));
}

//! @param  src     Move source
Image::Image(Image && src)
    : device_(std::move(src.device_))
    , info_(src.info_)
    , allocation_(std::move(src.allocation_))
    , image_(std::move(src.image_))
    , view_(std::move(src.view_))
{
}

//! @param  rhs     Move source
Image & Image::operator =(Image && rhs)
{
    if (this != &rhs)
    {
        device_         = std::move(rhs.device_);
        info_           = rhs.info_;
        allocation_     = std::move(rhs.allocation_);
        image_          = std::move(rhs.image_);
        view_           = std::move(rhs.view_);
    }
    return *this;
}

//! This function returns the number of mip levels needed to reach a 1x1 texture, assuming that the values are integers and the
//! length of a side is computed as: Length<sub>i</sub> = Length<sub>i-1</sub> > 1 ? Length<sub>i-1</sub> / 2 : 1
//!
//! @param  width       Width of the image
//! @param  height      Height of the image
//!
//! @return number of levels
uint32_t Image::computeMaxMipLevels(uint32_t width, uint32_t height)
{
    return static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
}

//! @param  device              Logical device associated with the image
//! @param  info                Creation info
//! @param  src                 Image data
//! @param  size                Size of image data
//! @param  aspect              Image aspect
HostImage::HostImage(std::shared_ptr<Device>     device,
                     vk::ImageCreateInfo const & info,
                     void const *                src /*= nullptr*/,
                     size_t                      size /*= 0*/,
                     vk::ImageAspectFlags        aspect /*= vk::ImageAspectFlagBits::eColor*/)
    : Image(device,
            info,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            aspect)
{
    if (src && size > 0)
        set(src, 0, size);
}

//! @param  src         Source data
//! @param  offset      Offset to the start of the image in the source data
//! @param  size        Size of image data
void HostImage::set(void const * src, size_t offset, size_t size)
{
    char * data = (char *)device_->mapMemory(allocation(), 0, size, vk::MemoryMapFlags());
    memcpy(data + offset, src, size);
    device_->unmapMemory(allocation());
}

//! @param  device              Logical device associated with the image
//! @param  info                Creation info
//! @param  aspect              Image aspect
LocalImage::LocalImage(std::shared_ptr<Device> device,
                       vk::ImageCreateInfo     info,
                       vk::ImageAspectFlags    aspect /*= vk::ImageAspectFlagBits::eColor*/)
    : Image(device, info, vk::MemoryPropertyFlagBits::eDeviceLocal, aspect)
{
}

//! @param  device              Logical device associated with the image
//! @param  commandPool         Command buffer allocator
//! @param  queue               Queue used to initialize the image
//! @param  info                Creation info
//! @param  src                 Image data
//! @param  size                Size of image data
//! @param  aspect              Image aspect
LocalImage::LocalImage(std::shared_ptr<Device> device,
                       vk::CommandPool const & commandPool,
                       vk::Queue const &       queue,
                       vk::ImageCreateInfo     info,
                       void const *            src,
                       size_t                  size,
                       vk::ImageAspectFlags    aspect /*= vk::ImageAspectFlagBits::eColor*/)
    : Image(device, info, vk::MemoryPropertyFlagBits::eDeviceLocal, aspect)
{
    set(commandPool, queue, src, size);
}

//! @param  commandPool         Command buffer allocator
//! @param  queue               Queue used to initialize the image
//! @param  src                 Image data
//! @param  size                Size of image data
void LocalImage::set(vk::CommandPool const & commandPool,
                     vk::Queue const &       queue,
                     void const *            src,
                     size_t                  size)
{
    // Transition to transfer dst for copy
    transitionLayout(commandPool,
                     queue,
                     vk::ImageLayout::eUndefined,
                     vk::ImageLayout::eTransferDstOptimal);

    // Copy the data to the image using a staging buffer
    HostBuffer staging(device_,
                       size,
                       vk::BufferUsageFlagBits::eTransferSrc,
                       src);
    copy(commandPool, queue, staging);

    // If there are mip levels, then generate them. Otherwise, just go ahead and transition the image to Shader read-only
    if (info_.mipLevels > 1)
    {
        generateMipmaps(commandPool, queue);
    }
    else
    {
        transitionLayout(commandPool,
                         queue,
                         vk::ImageLayout::eTransferDstOptimal,
                         vk::ImageLayout::eShaderReadOnlyOptimal);
    }
}

//! @param  commandPool     Command buffer allocator
//! @param  queue           Queue used to initialize the image
//! @param  buffer          Image data
void LocalImage::copy(vk::CommandPool const & commandPool,
                      vk::Queue const &       queue,
                      vk::Buffer const &      buffer)
{
    executeOnceSynched(device_,
                       commandPool,
                       queue,
                       [this, &buffer] (vk::CommandBuffer & commands) {
                           vk::BufferImageCopy region(0,
                                                      0,
                                                      0,
                                                      vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
                                                      { 0, 0, 0 },
                                                      { info_.extent.width, info_.extent.height, 1 });
                           commands.copyBufferToImage(buffer, *image_, vk::ImageLayout::eTransferDstOptimal, region);
                       });
}

//! @param  commandPool     Command buffer allocator
//! @param  queue           Queue used to initialize the image
//! @param  oldLayout       Current layout
//! @param  newLayout       New layout
void LocalImage::transitionLayout(vk::CommandPool const & commandPool,
                                  vk::Queue const &       queue,
                                  vk::ImageLayout         oldLayout,
                                  vk::ImageLayout         newLayout)
{
    vk::AccessFlags        srcAccessMask;
    vk::AccessFlags        dstAccessMask;
    vk::PipelineStageFlags srcStage;
    vk::PipelineStageFlags dstStage;
    vk::ImageAspectFlags   aspectMask;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        srcAccessMask = vk::AccessFlags();
        dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        srcStage      = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage      = vk::PipelineStageFlagBits::eTransfer;
        aspectMask    = vk::ImageAspectFlagBits::eColor;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        dstAccessMask = vk::AccessFlagBits::eShaderRead;
        srcStage      = vk::PipelineStageFlagBits::eTransfer;
        dstStage      = vk::PipelineStageFlagBits::eFragmentShader;
        aspectMask    = vk::ImageAspectFlagBits::eColor;
    }
    else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        srcAccessMask = vk::AccessFlags();
        dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        srcStage      = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage      = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        aspectMask    = vk::ImageAspectFlagBits::eDepth;
        if (info_.format == vk::Format::eD32SfloatS8Uint || info_.format == vk::Format::eD24UnormS8Uint)
            aspectMask |= vk::ImageAspectFlagBits::eStencil;
    }
    else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eColorAttachmentOptimal)
    {
        srcAccessMask = vk::AccessFlags();
        dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
        srcStage      = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage      = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        aspectMask    = vk::ImageAspectFlagBits::eColor;
    }
    else
    {
        throw std::invalid_argument("Vkx::Image::transitionLayout: unsupported layout transition");
    }

    vk::ImageMemoryBarrier barrier(srcAccessMask,
                                   dstAccessMask,
                                   oldLayout,
                                   newLayout,
                                   VK_QUEUE_FAMILY_IGNORED,
                                   VK_QUEUE_FAMILY_IGNORED,
                                   *image_,
                                   vk::ImageSubresourceRange(aspectMask, 0, info_.mipLevels, 0, 1));

    executeOnceSynched(device_,
                       commandPool,
                       queue,
                       [srcStage, dstStage, &barrier] (vk::CommandBuffer & commands) {
                           commands.pipelineBarrier(srcStage, dstStage, {}, nullptr, nullptr, barrier);
                       });
}

//! @param  commandPool         Command buffer allocator
//! @param  queue               Queue used to initialize the image
void LocalImage::generateMipmaps(vk::CommandPool const & commandPool,
                                 vk::Queue const &       queue)
{
    // Check if image format supports blitting with linear filtering
    vk::FormatProperties formatProperties = device_->physical()->getFormatProperties(info_.format);
    if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
        throw std::runtime_error("texture image format does not support linear blitting!");
    executeOnceSynched(device_,
                       commandPool,
                       queue,
                       [this] (vk::CommandBuffer & commands) {
                           vk::ImageMemoryBarrier barrier(vk::AccessFlags(),
                                                          vk::AccessFlags(),
                                                          vk::ImageLayout::eUndefined,
                                                          vk::ImageLayout::eUndefined,
                                                          VK_QUEUE_FAMILY_IGNORED,
                                                          VK_QUEUE_FAMILY_IGNORED,
                                                          *image_,
                                                          vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

                           int32_t mipWidth  = info_.extent.width;
                           int32_t mipHeight = info_.extent.height;

                           for (uint32_t i = 1; i < info_.mipLevels; i++)
                           {
                               int32_t previousWidth  = mipWidth;
                               int32_t previousHeight = mipHeight;

                               if (mipWidth > 1)
                                   mipWidth /= 2;
                               if (mipHeight > 1)
                                   mipHeight /= 2;

                               // Transition the layout for the previous mip level to transfer src
                               barrier.subresourceRange.setBaseMipLevel(i - 1);
                               barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
                               barrier.setNewLayout(vk::ImageLayout::eTransferSrcOptimal);
                               barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
                               barrier.setDstAccessMask(vk::AccessFlagBits::eTransferRead);

                               commands.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                                        vk::PipelineStageFlagBits::eTransfer,
                                                        {},
                                                        nullptr,
                                                        nullptr,
                                                        barrier);

                               // Blit the previous mip level to the current mip level
                               commands.blitImage(*image_,
                                                  vk::ImageLayout::eTransferSrcOptimal,
                                                  *image_,
                                                  vk::ImageLayout::eTransferDstOptimal,
                                                  vk::ImageBlit(
                                                      vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i - 1, 0, 1),
                                                      {{{ 0, 0, 0 }, { previousWidth, previousHeight, 1 } } },
                                                      vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i, 0, 1),
                                                      {{{ 0, 0, 0 }, { mipWidth, mipHeight, 1 } } }),
                                                  vk::Filter::eLinear);
                           }

                           // Transition the final mip level to transfer src so that all levels can be transitioned to shader
                           // read-only in one shot
                           barrier.subresourceRange.setBaseMipLevel(info_.mipLevels - 1);
                           barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
                           barrier.setNewLayout(vk::ImageLayout::eTransferSrcOptimal);
                           barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
                           barrier.setDstAccessMask(vk::AccessFlagBits::eTransferRead);
                           commands.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                                    vk::PipelineStageFlagBits::eTransfer,
                                                    {},
                                                    nullptr,
                                                    nullptr,
                                                    barrier);

                           // Transition all mip levels to shader read-only
                           barrier.subresourceRange.setBaseMipLevel(0);
                           barrier.subresourceRange.setLevelCount(info_.mipLevels);
                           barrier.setOldLayout(vk::ImageLayout::eTransferSrcOptimal);
                           barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
                           barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
                           barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
                           commands.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                                    vk::PipelineStageFlagBits::eFragmentShader,
                                                    {},
                                                    nullptr,
                                                    nullptr,
                                                    barrier);
                       });
}

//! @param  device              Logical device associated with the image
//! @param  commandPool         Command buffer allocator
//! @param  queue               Queue used to initialize the image
//! @param  info                Creation info
DepthImage::DepthImage(std::shared_ptr<Device> device,
                       vk::CommandPool const & commandPool,
                       vk::Queue const &       queue,
                       vk::ImageCreateInfo     info)
    : LocalImage(device, info, vk::ImageAspectFlagBits::eDepth)
{
    transitionLayout(commandPool,
                     queue,
                     vk::ImageLayout::eUndefined,
                     vk::ImageLayout::eDepthStencilAttachmentOptimal);
}

//! @param  device              Logical device associated with the image
//! @param  commandPool         Command buffer allocator
//! @param  queue               Queue used to initialize the image
//! @param  info                Creation info
ResolveImage::ResolveImage(std::shared_ptr<Device> device,
                           vk::CommandPool const & commandPool,
                           vk::Queue const &       queue,
                           vk::ImageCreateInfo     info)
    : LocalImage(device, info)
{
    transitionLayout(commandPool,
                     queue,
                     vk::ImageLayout::eUndefined,
                     vk::ImageLayout::eColorAttachmentOptimal);
}
} // namespace Vkx
