#if !defined(VKX_DEVICE_H)
#define VKX_DEVICE_H

#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Vkx
{
class Instance;
class PhysicalDevice;

//! A destructible extension to vk::Device.
//!
//! @note   A Device can be moved, but cannot be copied.

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

//! A destructible extension to vk::PhysicalDevice.
//!
//! @note   A PhysicalDevice can be moved, but cannot be copied.
class PhysicalDevice : public vk::PhysicalDevice
{
public:

    //! Constructor.
    //!
    //! @param  instance        Instance providing the physical device
    //! @param  chooser         Chooses which vk::PhysicalDevice this object wraps
    PhysicalDevice(std::shared_ptr<Instance> &                                                instance,
                   vk::SurfaceKHR                                                             surface,
                   std::function<vk::PhysicalDevice(std::vector<vk::PhysicalDevice> const &)> chooser);

    PhysicalDevice(PhysicalDevice && src);

    PhysicalDevice & operator =(PhysicalDevice && rhs);

    //! Returns the instance associated with this physical device.
    std::shared_ptr<Instance> instance() const { return instance_; }

    //! Returns the surface associated with this physical device.
    vk::SurfaceKHR surface() const { return surface_; }

private:
    std::shared_ptr<Instance> instance_;
    vk::SurfaceKHR surface_;
};
} // namespace Vkx

#endif // !defined(VKX_DEVICE_H)
