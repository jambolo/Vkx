#if !defined(VKX_INSTANCE_H)
#define VKX_INSTANCE_H

#pragma once

#include <vulkan/vulkan.hpp>

namespace Vkx
{
//! A RAII extension to vk::Instance.
//!
//! @note   Instances can be moved, but cannot be copied.

class Instance : public vk::Instance
{
public:
    //! Constructor.
    Instance(vk::InstanceCreateInfo const & info);

    //! Move constructor.
    Instance(Instance && src);

    ~Instance();

    //! Move-assignment operator
    Instance & operator =(Instance && rhs);

private:
    // Non-copyable
    Instance(Instance const &) = delete;
    Instance & operator =(Instance const &) = delete;
};
} // namespace Vkx

#endif // !defined(VKX_INSTANCE_H)
