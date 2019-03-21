#if !defined(VKX_IMAGE_H)
#define VKX_IMAGE_H

#pragma once

#include <vulkan/vulkan.hpp>
#include <Vkx/Device.h>
#include <Vkx/Vkx.h>

namespace Vkx
{
//! An extension to vk::Image that supports ownership of the memory and the view.
//!
//! @note   Instances can be moved, but cannot be copied.
class Image
{
public:
    //! Constructor.
    Image() = default;

    //! Constructor.
    Image(std::shared_ptr<Device>     device,
          vk::ImageCreateInfo const & info,
          vk::MemoryPropertyFlags     memoryProperties,
          vk::ImageAspectFlags        aspect);

    //! Move constructor
    Image(Image && src);

    //! Destructor.
    virtual ~Image()
    {
        device_->destroy(view_);
        device_->destroy(image_);
        device_->free(allocation_);
    }

    //! Move-assignment operator
    Image & operator =(Image && rhs);

    //! Implicitly converts into a vk::Image.
    operator vk::Image() const { return image_; }

    //! Returns the DeviceMemory handle.
    vk::DeviceMemory allocation() const { return allocation_; }

    //! Returns the view
    vk::ImageView view() const { return view_; }

    //! Returns the creation info.
    vk::ImageCreateInfo info() const { return info_; }

    //! Returns the maximum number of mip levels needed for the given with and height.
    static uint32_t computeMaxMipLevels(uint32_t width, uint32_t height);

protected:
    std::shared_ptr<Device> device_;    //!< Device associated with this image
    vk::ImageCreateInfo info_;          //!< Info about the image
    vk::DeviceMemory allocation_;       //!< The image data
    vk::Image image_;                   //!< The image
    vk::ImageView view_;                //!< The image view
};

//! An Image that is visible to the CPU and is automatically kept in sync (eHostVisible | eHostCoherent).
class HostImage : public Image
{
public:
    //! Constructor.
    HostImage() = default;

    //! Constructor.
    HostImage(std::shared_ptr<Device>     device,
              vk::ImageCreateInfo const & info,
              void const *                src = nullptr,
              size_t                      size = 0,
              vk::ImageAspectFlags        aspect = vk::ImageAspectFlagBits::eColor);

    //! Copies image data from CPU memory into the image.
    void set(void const * src, size_t offset, size_t size);
};

//! An Image that is accessible only to the GPU (eDeviceLocal).
class LocalImage : public Image
{
public:
    //! Constructor.
    LocalImage() = default;

    //! Constructor.
    LocalImage(std::shared_ptr<Device>    device,
               vk::ImageCreateInfo        info,
               vk::ImageAspectFlags       aspect = vk::ImageAspectFlagBits::eColor);

    //! Constructor.
    LocalImage(std::shared_ptr<Device>    device,
               vk::CommandPool const &    commandPool,
               vk::Queue const &          queue,
               vk::ImageCreateInfo        info,
               void const *               src,
               size_t                     size,
               vk::ImageAspectFlags       aspect = vk::ImageAspectFlagBits::eColor);

    //! Copies data from CPU memory into the image
    void set(vk::CommandPool const &    commandPool,
             vk::Queue const &          queue,
             void const *               src,
             size_t                     size);

    //! Copies data from a buffer into the image
    void copy(vk::CommandPool const & commandPool,
              vk::Queue const &       queue,
              vk::Buffer const &      buffer);

    //! Transitions the image's layout
    void transitionLayout(vk::CommandPool const & commandPool,
                          vk::Queue const &       queue,
                          vk::ImageLayout         oldLayout,
                          vk::ImageLayout         newLayout);

    //! Generates mipmaps for the image
    void generateMipmaps(vk::CommandPool const &    commandPool,
                         vk::Queue const &          queue);
};

//! A LocalImage for use as a depth buffer (vk::ImageAspect::eDEPTH).
class DepthImage : public LocalImage
{
public:
    //! Constructor.
    DepthImage() = default;

    //! Constructor.
    DepthImage(std::shared_ptr<Device>    device,
               vk::CommandPool const &    commandPool,
               vk::Queue const &          queue,
               vk::ImageCreateInfo        info);
};

//! A LocalImage for use as a MSAA buffer (vk::ImageLayout::eColorAttachmentOptimal).
class ResolveImage : public LocalImage
{
public:
    //! Constructor.
    ResolveImage() = default;

    //! Constructor.
    ResolveImage(std::shared_ptr<Device>    device,
                 vk::CommandPool const &    commandPool,
                 vk::Queue const &          queue,
                 vk::ImageCreateInfo        info);
};
}

#endif // !defined(VKX_IMAGE_H)
