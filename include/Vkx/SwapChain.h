#if !defined(VKX_SWAPCHAIN_H)
#define VKX_SWAPCHAIN_H

#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Vkx
{
class Device;

class SwapChain
{
public:
    static int constexpr MAX_LATENCY = 3;

    SwapChain(std::shared_ptr<Device>      device,
              vk::SurfaceFormatKHR const & surfaceFormat,
              vk::Extent2D                 extent,
              uint32_t                     graphicsFamily,
              uint32_t                     presentFamily,
              vk::PresentModeKHR           presentMode);

    ~SwapChain();

    //! Advances to the next swap chain image.
    uint32_t swap();

    //! Returns the image format.
    vk::Format format() const { return format_; }

    //! Returns the extent.
    vk::Extent2D const & extent() const { return extent_; }

    //! Returns the number of images.
    size_t size() const { return views_.size(); }

    //! Returns the specified image view.
    vk::ImageView & view(size_t i) { return views_[i]; }

    //! Returns the image-available semaphore for the current frame.
    vk::Semaphore & imageAvailable() { return *imageAvailableSemaphores_[currentFrame_]; }

    //! Returns the render-finished semaphore for the current frame.
    vk::Semaphore & renderFinished() { return *renderFinishedSemaphores_[currentFrame_]; }

    //! Returns the in-flight fence for the current frame.
    vk::Fence & inFlight() { return *inFlightFences_[currentFrame_]; }

    //! Implicitly converts to the underlying vk::SwapchainKHR object
    operator vk::SwapchainKHR() { return swapChain_; }

private:
    std::shared_ptr<Device> device_;
    vk::SwapchainKHR swapChain_;
    vk::Format format_;
    std::vector<vk::ImageView> views_;
    vk::Extent2D extent_;
    std::vector<vk::UniqueSemaphore> imageAvailableSemaphores_;
    std::vector<vk::UniqueSemaphore> renderFinishedSemaphores_;
    std::vector<vk::UniqueFence> inFlightFences_;
    int currentFrame_ = 0;
};
} // namespace Vkx

#endif // !defined(VKX_SWAPCHAIN_H)
