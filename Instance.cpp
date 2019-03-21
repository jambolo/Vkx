#include "Instance.h"

#include <vulkan/vulkan.hpp>

namespace Vkx
{
//! @param  info    Creation info
Instance::Instance(vk::InstanceCreateInfo const & info)
    : vk::Instance(vk::createInstance(info))
{
}

Instance::Instance(Instance && src)
    : vk::Instance(src)
{
}

Instance::~Instance()
{
    vk::Instance::destroy();
}

Vkx::Instance & Instance::operator =(Instance && rhs)
{
    vk::Instance::operator =(rhs);
    return *this;
}
} // namespace Vkx
