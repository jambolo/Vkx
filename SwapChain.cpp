#include "SwapChain.h"

#include "Device.h"

#include <memory>
#include <vulkan/vulkan.hpp>

namespace Vkx
{
SwapChain::SwapChain(std::shared_ptr<Device>      device,
                     vk::SurfaceFormatKHR const & surfaceFormat,
                     vk::Extent2D                 extent,
                     uint32_t                     graphicsFamily,
                     uint32_t                     presentFamily,
                     vk::PresentModeKHR           presentMode)
    : device_(device)
{
    std::shared_ptr<PhysicalDevice> physicalDevice = device->physical();
    vk::SurfaceKHR surface = physicalDevice->surface();
    vk::SurfaceCapabilitiesKHR capabilities = physicalDevice->getSurfaceCapabilitiesKHR(surface);
    uint32_t nImages = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && nImages > capabilities.maxImageCount)
        nImages = capabilities.maxImageCount;

    vk::SwapchainCreateInfoKHR createInfo({},
                                          surface,
                                          nImages,
                                          surfaceFormat.format,
                                          surfaceFormat.colorSpace,
                                          extent,
                                          1,
                                          vk::ImageUsageFlagBits::eColorAttachment);

    uint32_t queueFamilyIndices[] =
    {
        graphicsFamily,
        presentFamily
    };

    if (graphicsFamily != presentFamily)
    {
        createInfo.imageSharingMode      = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices   = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.presentMode  = presentMode;
    createInfo.clipped      = VK_TRUE;

    swapChain_ = device->createSwapchainKHRUnique(createInfo);

    std::vector<vk::Image> images = device->getSwapchainImagesKHR(*swapChain_);
    format_ = surfaceFormat.format;
    extent_ = extent;
    views_.reserve(images.size());
    for (auto const & image : images)
    {
        views_.push_back(
            device->createImageViewUnique(
                vk::ImageViewCreateInfo({},
                                        image,
                                        vk::ImageViewType::e2D,
                                        format_,
                                        vk::ComponentMapping(),
                                        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))));
    }

    imageAvailableSemaphores_.reserve(MAX_LATENCY);
    renderFinishedSemaphores_.reserve(MAX_LATENCY);
    inFlightFences_.reserve(MAX_LATENCY);

    for (int i = 0; i < MAX_LATENCY; ++i)
    {
        imageAvailableSemaphores_.push_back(device_->createSemaphoreUnique(vk::SemaphoreCreateInfo()));
        renderFinishedSemaphores_.push_back(device_->createSemaphoreUnique(vk::SemaphoreCreateInfo()));
        inFlightFences_.push_back(device_->createFenceUnique(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled)));
    }
}

//! @param  src     Move source
SwapChain::SwapChain(SwapChain && src)
    : device_(std::move(src.device_))
    , swapChain_(std::move(src.swapChain_))
    , format_(src.format_)
    , views_(std::move(src.views_))
    , extent_(src.extent_)
    , imageAvailableSemaphores_(std::move(src.imageAvailableSemaphores_))
    , renderFinishedSemaphores_(std::move(src.renderFinishedSemaphores_))
    , inFlightFences_(std::move(src.inFlightFences_))
    , currentFrame_(src.currentFrame_)
{
}

//! @param  src     Move source
Vkx::SwapChain & SwapChain::operator =(SwapChain && rhs)
{
    if (this != &rhs)
    {
        device_ = std::move(rhs.device_);
        swapChain_ = std::move(rhs.swapChain_);
        format_ = std::move(rhs.format_);
        views_ = std::move(rhs.views_);
        extent_ = std::move(rhs.extent_);
        imageAvailableSemaphores_ = std::move(rhs.imageAvailableSemaphores_);
        renderFinishedSemaphores_ = std::move(rhs.renderFinishedSemaphores_);
        inFlightFences_ = std::move(rhs.inFlightFences_);
        currentFrame_ = std::move(rhs.currentFrame_);
    }
    return *this;
}

//! @warning    A std::runtime_error is thrown if the swap fails.
//!

uint32_t SwapChain::swap()
{
    if (!swapChain_)
        throw std::runtime_error("SwapChain::swap: invalidated swap chain!");
    
    currentFrame_ = (currentFrame_ + 1) % MAX_LATENCY;
    device_->waitForFences(1, &(*inFlightFences_[currentFrame_]), VK_TRUE, std::numeric_limits<uint64_t>::max());
    device_->resetFences(1, &(*inFlightFences_[currentFrame_]));

    vk::ResultValue<uint32_t> result = device_->acquireNextImageKHR(*swapChain_,
                                                                    std::numeric_limits<uint64_t>::max(),
                                                                    *imageAvailableSemaphores_[currentFrame_],
                                                                    nullptr);

    if (result.result != vk::Result::eSuccess && result.result != vk::Result::eSuboptimalKHR)
        throw std::runtime_error("SwapChain::swap: failed to acquire swap chain image!");
    return result.value;
}
} // namespace Vkx
