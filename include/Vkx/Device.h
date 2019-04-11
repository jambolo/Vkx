#if !defined(VKX_DEVICE_H)
#define VKX_DEVICE_H

#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <Vkx/Instance.h>

namespace Vkx
{
class PhysicalDevice;

//! A RAII extension to vk::Device.
//!
//! @note   Instances can be moved, but cannot be copied.

class Device : public vk::Device
{
public:
    //! Constructor.
    Device(std::shared_ptr<PhysicalDevice> physicalDevice, vk::DeviceCreateInfo const & info);

    //! Move constructor.
    Device(Device && src);

    ~Device();

    //! Move-assignment operator
    Device & operator =(Device && rhs);

    //! Returns the physical device this device is associated with.
    std::shared_ptr<PhysicalDevice> physical() const { return physicalDevice_; }

private:
    // Non-copyable
    Device(Device const &) = delete;
    Device & operator =(Device const &) = delete;

    std::shared_ptr<PhysicalDevice> physicalDevice_;
};

//! A RAII extension to vk::PhysicalDevice.
class PhysicalDevice : public vk::PhysicalDevice
{
public:

    //! Constructor.
    //!
    //! @param  instance        Instance providing the physical device
    //! @param  chooser         Chooses which vk::PhysicalDevice this object wraps
    PhysicalDevice(std::shared_ptr<Instance> &                                                instance,
                   std::function<vk::PhysicalDevice(std::vector<vk::PhysicalDevice> const &)> chooser);

    PhysicalDevice::PhysicalDevice(PhysicalDevice && src);

    PhysicalDevice & PhysicalDevice::operator =(PhysicalDevice && rhs);

    //! Returns the instance this physical device is associated with.
    std::shared_ptr<Instance> instance() const { return instance_; }

private:
    std::shared_ptr<Instance> instance_;
};
} // namespace Vkx

#endif // !defined(VKX_DEVICE_H)
