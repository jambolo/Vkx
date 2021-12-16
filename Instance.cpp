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
    static_cast<vk::Instance &>(src) = nullptr;
}

Instance::~Instance()
{
    vk::Instance::destroy();
}

Vkx::Instance & Instance::operator =(Instance && rhs)
{
    if (this != &rhs)
    {
        vk::Instance::destroy();
        
        vk::Instance::operator =(rhs);
        
        static_cast<vk::Instance &>(rhs) = nullptr;
    }
    return *this;
}
} // namespace Vkx
