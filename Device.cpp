#include "Device.h"

#include <vulkan/vulkan.hpp>

namespace Vkx
{
//! @param  physicalDevice  Physical device to be associated with this device
//! @param  info            Creation info
    Device::Device(std::shared_ptr<PhysicalDevice> physicalDevice, vk::DeviceCreateInfo const & info)
    : vk::Device(physicalDevice->createDevice(info))
    , physicalDevice_(physicalDevice)
{
}

//! @param  src     Move source
Device::Device(Device && src)
{
    physicalDevice_ = std::move(src.physicalDevice_);
}

Device::~Device()
{
    vk::Device::destroy();
}

//! @param  rhs     Move source
Device & Device::operator =(Device && rhs)
{
    if (this != &rhs)
        physicalDevice_ = std::move(rhs.physicalDevice_);
    return *this;
}
} // namespace Vkx
