#include "Buffer.h"

#include "Vkx.h"

#include <vulkan/vulkan.hpp>

namespace Vkx
{
//! @param  device              Logical device associated with the buffer
//! @param  size                Nominal size of the buffer
//! @param  usage               Usage flags
//! @param  sharingMode         Sharing mode
//! @param  memoryProperties    Memory properties
//! @param  sharingMode         Sharing mode flag (default: eExclusive)
//!
//! @warning       A std::runtime_error is thrown if the buffer cannot be created and allocated
//! @todo       "... you're not supposed to actually call vkAllocateMemory for every individual buffer. ... The right way to
//!             allocate memory for a large number of objects at the same time is to create a custom allocator that splits up a
//!             single allocation among many different objects by using the offset parameters that we've seen in many functions."

Buffer::Buffer(std::shared_ptr<Device> device,
               size_t                  size,
               vk::BufferUsageFlags    usage,
               vk::MemoryPropertyFlags memoryProperties,
               vk::SharingMode         sharingMode /*= vk::SharingMode::eExclusive*/)
    : device_(device)
{
    buffer_ = device_->createBuffer(vk::BufferCreateInfo({}, size, usage, sharingMode));

    vk::MemoryRequirements requirements = device_->getBufferMemoryRequirements(buffer_);
    uint32_t memoryType = Vkx::findAppropriateMemoryType(device_->physical(),
                                                         requirements.memoryTypeBits,
                                                         memoryProperties);

    allocation_ = device_->allocateMemory(vk::MemoryAllocateInfo(requirements.size, memoryType));
    device_->bindBufferMemory(buffer_, allocation_, 0);
}

//! @param  src     Move source
Buffer::Buffer(Buffer && src)
    : device_(std::move(src.device_))
    , allocation_(src.allocation_)
    , buffer_(src.buffer_)
{
    src.allocation_ = nullptr;
    src.buffer_ = nullptr;
}

Buffer::~Buffer()
{
    if (device_)
    {
        device_->destroy(buffer_);
        device_->free(allocation_);
    }
}
vk::UniqueBuffer x;
//! @param  rhs     Move source
Buffer & Buffer::operator =(Buffer && rhs)
{
    if (this != &rhs)
    {
        device_     = std::move(rhs.device_);
        allocation_ = rhs.allocation_;
        buffer_     = rhs.buffer_;
        rhs.allocation_ = nullptr;
        rhs.buffer_ = nullptr;
    }
    return *this;
}

//! @param  device          Logical device associated with the buffer
//! @param  size            Nominal size of the buffer
//! @param  usage           Usage flags
//! @param  src             Data to be copied into the buffer, or nullptr if nothing to copy (default: nullptr)
//! @param  sharingMode     Sharing mode flag (default: eExclusive)
HostBuffer::HostBuffer(std::shared_ptr<Device> device,
                       size_t                  size,
                       vk::BufferUsageFlags    usage,
                       void const *            src /*= nullptr*/,
                       vk::SharingMode         sharingMode /*= vk::SharingMode::eExclusive*/)
    : Buffer(device,
             size,
             usage,
             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
             sharingMode)
{
    if (src)
        set(0, src, size);
}

//! @param  offset  Where in the buffer to put the copied data
//! @param  src     Data to be copied into the buffer
//! @param  size    Size of the data to copy
void HostBuffer::set(size_t offset, void const * src, size_t size)
{
    char * data = (char *)device_->mapMemory(allocation(), 0, size, vk::MemoryMapFlags());
    memcpy(data + offset, src, size);
    device_->unmapMemory(allocation());
}

//! @param  device          Logical device associated with the buffer
//! @param  size            Nominal size of the buffer
//! @param  usage           Usage flags
//! @param  sharingMode     Sharing mode flag (default: eExclusive)
LocalBuffer::LocalBuffer(std::shared_ptr<Device> device,
                         size_t                  size,
                         vk::BufferUsageFlags    usage,
                         vk::SharingMode         sharingMode /*= vk::SharingMode::eExclusive*/)
    : Buffer(device,
             size,
             usage | vk::BufferUsageFlagBits::eTransferDst,
             vk::MemoryPropertyFlagBits::eDeviceLocal,
             sharingMode)
{
}

//! @param  device          Logical device associated with the buffer
//! @param  commandPool     Command pool used to initialize the buffer
//! @param  queue           Queue used to initialize the buffer
//! @param  size            Nominal size of the buffer
//! @param  usage           Usage flags
//! @param  src             Data to be copied into the buffer
//! @param  sharingMode     Sharing mode flag (default: eExclusive)
LocalBuffer::LocalBuffer(std::shared_ptr<Device> device,
                         vk::CommandPool const & commandPool,
                         vk::Queue const &       queue,
                         size_t                  size,
                         vk::BufferUsageFlags    usage,
                         void const *            src,
                         vk::SharingMode         sharingMode /*= vk::SharingMode::eExclusive*/)
    : Buffer(device,
             size,
             usage | vk::BufferUsageFlagBits::eTransferDst,
             vk::MemoryPropertyFlagBits::eDeviceLocal,
             sharingMode)
{
    set(commandPool, queue, src, size);
}

//! @param  commandPool     Command pool used to copy data into the buffer
//! @param  queue           Queue used to copy data into the buffer
//! @param  src             Data to be copied into the buffer
//! @param  size            Size of the data to copy
void LocalBuffer::set(vk::CommandPool const & commandPool,
                      vk::Queue const &       queue,
                      void const *            src,
                      size_t                  size)
{
    HostBuffer staging(device_,
                       size,
                       vk::BufferUsageFlagBits::eTransferSrc,
                       src);
    copySynched(commandPool, queue, staging, size);
}

void LocalBuffer::copySynched(vk::CommandPool const & commandPool,
                              vk::Queue const &       queue,
                              Buffer &                src,
                              size_t                  size)
{
    executeOnceSynched(device_, commandPool, queue, [&src, this, size] (vk::CommandBuffer commands) {
                           commands.copyBuffer(src, this->buffer_, vk::BufferCopy(0, 0, size));
                       });
}
} // namespace Vkx
