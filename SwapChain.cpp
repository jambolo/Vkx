#include "SwapChain.h"

#include "Device.h"

#include <memory>

namespace Vkx
{
SwapChain::SwapChain(vk::SurfaceKHR const &             surface,
                     vk::SurfaceFormatKHR const &       surfaceFormat,
                     vk::Extent2D                       extent,
                     uint32_t                           graphicsFamily,
                     uint32_t                           presentFamily,
                     vk::PresentModeKHR                 presentMode,
                     std::shared_ptr<Device>            device)
    : device_(device)
{
    vk::SurfaceCapabilitiesKHR capabilities = device->physical()->getSurfaceCapabilitiesKHR(surface);
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

    swapChain_ = device->createSwapchainKHR(createInfo);

    std::vector<vk::Image> images = device->getSwapchainImagesKHR(swapChain_);
    format_ = surfaceFormat.format;
    extent_ = extent;
    views_.reserve(images.size());
    for (auto const & image : images)
    {
        views_.push_back(
            device->createImageView(
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

SwapChain::~SwapChain()
{
    for (auto const & view : views_)
    {
        device_->destroy(view);
    }
    device_->destroy(swapChain_);
}

uint32_t SwapChain::swap()
{
    currentFrame_ = (currentFrame_ + 1) % MAX_LATENCY;
    device_->waitForFences(1, &(*inFlightFences_[currentFrame_]), VK_TRUE, std::numeric_limits<uint64_t>::max());
    device_->resetFences(1, &(*inFlightFences_[currentFrame_]));

    vk::ResultValue<uint32_t> result = device_->acquireNextImageKHR(swapChain_,
                                                                    std::numeric_limits<uint64_t>::max(),
                                                                    *imageAvailableSemaphores_[currentFrame_],
                                                                    nullptr);

    if (result.result != vk::Result::eSuccess && result.result != vk::Result::eSuboptimalKHR)
        throw std::runtime_error("SwapChain::swap: failed to acquire swap chain image!");
    return result.value;
}
} // namespace Vkx
