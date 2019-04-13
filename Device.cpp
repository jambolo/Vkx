#include "Device.h"

#include "Instance.h"

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
    : vk::Device(src)
    , physicalDevice_(std::move(src.physicalDevice_))
{
    static_cast<vk::Device &>(src) = nullptr;
}

Device::~Device()
{
    vk::Device::destroy();
}

//! @param  rhs     Move source
Device & Device::operator =(Device && rhs)
{
    if (this != &rhs)
    {
        vk::Device::operator =(rhs);
        static_cast<vk::Device &>(rhs) = nullptr;
        physicalDevice_ = std::move(rhs.physicalDevice_);
    }
    return *this;
}

PhysicalDevice::PhysicalDevice(std::shared_ptr<Instance> &                                                instance,
                               std::function<vk::PhysicalDevice(std::vector<vk::PhysicalDevice> const &)> chooser)
    : vk::PhysicalDevice(chooser(instance->enumeratePhysicalDevices()))
    , instance_(instance)
{
}

PhysicalDevice::PhysicalDevice(PhysicalDevice && src)
    : vk::PhysicalDevice(src)
    , instance_(std::move(src.instance_))
{
    static_cast<vk::PhysicalDevice &>(src) = nullptr;
}

PhysicalDevice & PhysicalDevice::operator =(PhysicalDevice && rhs)
{
    if (&rhs != this)
    {
        vk::PhysicalDevice::operator =(rhs);
        static_cast<vk::PhysicalDevice &>(rhs) = nullptr;
        instance_ = std::move(rhs.instance_);
    }
    return *this;
}
} // namespace Vkx
