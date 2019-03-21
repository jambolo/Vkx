#pragma once

#if !defined(VKX_INSTANCE_H)
#define VKX_INSTANCE_H

#include <vulkan/vulkan.hpp>

namespace Vkx
{
//! A RAII extension to vk::Instance.
//!
//! @note   The class is movable, but not copyable.

class Instance : public vk::Instance
{
public:
    //! Constructor.
    Instance(vk::InstanceCreateInfo const & info);

    //! Move constructor.
    Instance(Instance && src) : vk::Instance(src) {}

    ~Instance();

    //! Move-assignment operator
    Instance & operator =(Instance && rhs) { vk::Instance::operator =(rhs); return *this; }

private:
    // Non-copyable
    Instance(Instance const &) = delete;
    Instance & operator =(Instance const &) = delete;

};
} // namespace Vkx

#endif // !defined(VKX_INSTANCE_H)
