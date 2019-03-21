#pragma once

#if !defined(VKX_BUFFER_H)
#define VKX_BUFFER_H

#include <vulkan/vulkan.hpp>
#include <Vkx/Device.h>
#include <Vkx/Vkx.h>

namespace Vkx
{
//! An extension of vk::Buffer that supports ownership of the memory.
//!
//! The buffer and its memory allocation (if any) are destroyed automatically when this object is destroyed.
//!
//! @note   Instances can be moved, but cannot be copied.

class Buffer
{
public:
    //! Constructor.
    Buffer() = default;

    //! Constructor.
    Buffer(std::shared_ptr<Device> device,
           size_t                  size,
           vk::BufferUsageFlags    usage,
           vk::MemoryPropertyFlags memoryProperties,
           vk::SharingMode         sharingMode = vk::SharingMode::eExclusive);

    //! Move constructor
    Buffer(Buffer && src);

    //! Destructor.
    virtual ~Buffer();

    //! Move-assignment operator
    Buffer & operator =(Buffer && rhs);

    //! Implicitly converts into a vk::Buffer.
    operator vk::Buffer() const { return buffer_; }

    //! Returns the DeviceMemory handle.
    vk::DeviceMemory allocation() const { return allocation_; }

protected:
    std::shared_ptr<Device> device_;    //!< Device associated with this buffer
    vk::DeviceMemory allocation_;       //!< %Buffer allocation
    vk::Buffer buffer_;                 //!< Vulkan buffer

private:
    // Non-copyable
    Buffer(Buffer &) = delete;
    Buffer & operator =(Buffer &) = delete;
};

//! A Buffer that is visible to the CPU and is automatically kept in sync (eHostVisible | eHostCoherent).
class HostBuffer : public Buffer
{
public:
    //! Constructor.
    HostBuffer() = default;

    //! Constructor.
    HostBuffer(std::shared_ptr<Device> device,
               size_t                  size,
               vk::BufferUsageFlags    usage,
               void const *            src         = nullptr,
               vk::SharingMode         sharingMode = vk::SharingMode::eExclusive);

    //! Copies CPU memory into the buffer
    void set(size_t offset, void const * src, size_t size);
};

//! A Buffer that is visible only to the GPU (eDeviceLocal).
class LocalBuffer : public Buffer
{
public:
    //! Constructor.
    LocalBuffer() = default;

    //! Constructor.
    LocalBuffer(std::shared_ptr<Device> device,
                size_t                  size,
                vk::BufferUsageFlags    usage,
                vk::SharingMode         sharingMode = vk::SharingMode::eExclusive);

    //! Constructor.
    LocalBuffer(std::shared_ptr<Device> device,
                vk::CommandPool const & commandPool,
                vk::Queue const &       queue,
                size_t                  size,
                vk::BufferUsageFlags    usage,
                void const *            src,
                vk::SharingMode         sharingMode = vk::SharingMode::eExclusive);

    //! Copies data from CPU memory into the buffer
    void set(vk::CommandPool const & commandPool,
             vk::Queue const &       queue,
             void const *            src,
             size_t                  size);

private:
    void copySynched(vk::CommandPool const & commandPool,
                     vk::Queue const &       queue,
                     Buffer &                src,
                     size_t                  size);
};
} // namespace Vkx

#endif // !defined(VKX_BUFFER_H)
