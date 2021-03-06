#include "Vkx.h"

#include "Buffer.h"

#include <vulkan/vulkan.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <vector>

namespace Vkx
{
//! @param  extensions
//! @param  name
bool extensionIsSupported(std::vector<vk::ExtensionProperties> & extensions, char const * name)
{
    std::vector<vk::ExtensionProperties>::const_iterator i;
    i = std::find_if(extensions.begin(),
                     extensions.end(),
                     [name] (vk::ExtensionProperties const & e) { return strcmp(name, e.extensionName) == 0; });
    return i != extensions.end();
}

//! @param  physicalDevice
//! @param  extensions
    bool allExtensionsSupported(vk::PhysicalDevice const &        physicalDevice,
                                std::vector<char const *> const & extensions)
{
    std::vector<vk::ExtensionProperties> available = physicalDevice.enumerateDeviceExtensionProperties(nullptr);
    for (auto const & required : extensions)
    {
        if (!extensionIsSupported(available, required))
            return false;
    }
    return true;
}

//! @param  layers
//! @param  name
bool layerIsAvailable(std::vector<vk::LayerProperties> layers, char const * name)
{
    std::vector<vk::LayerProperties>::const_iterator i;
    i = std::find_if(layers.begin(),
                     layers.end(),
                     [name] (vk::LayerProperties const & e) { return strcmp(name, e.layerName) == 0; });
    return i != layers.end();
}

//!
//! @param  requested
bool allLayersAvailable(std::vector<char const *> const & requested)
{
    std::vector<vk::LayerProperties> available = vk::enumerateInstanceLayerProperties();
    for (auto const & request : requested)
    {
        if (!layerIsAvailable(available, request))
            return false;
    }
    return true;
}

//! @param  path        Path to shader file
//! @param  device      Logical device managing the shader
//! @param  flags       Creation flags (default: none)
//!
//! @return shader module handle
//!
//! @warning   std::runtime_error is thrown if the file cannot be opened
vk::ShaderModule loadShaderModule(std::string const &         path,
                                  std::shared_ptr<Device>     device,
                                  vk::ShaderModuleCreateFlags flags /*= vk::ShaderModuleCreateFlags()*/)
{
    // Load the code
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Vkx::loadShaderModule: failed to open the file");
    size_t fileSize   = (size_t)file.tellg();
    size_t shaderSize = (fileSize + (sizeof(uint32_t) - 1)) / sizeof(uint32_t);
    std::vector<uint32_t> buffer(shaderSize);
    file.seekg(0);
    file.read((char *)buffer.data(), fileSize);

    // Create the module for the device
    vk::ShaderModule shaderModule = device->createShaderModule({ flags, buffer.size() * sizeof(uint32_t), buffer.data() });

    return shaderModule;
}

//! @param  physicalDevice      The physical device that will allocate the memory
//! @param  types               Acceptable memory types as determined by vk::Device::getBufferMemoryRequirements()
//! @param  properties          Necessary properties
//!
//! @return     index of the type of memory provided by the physical device that matches the request
//!
//! @warning    A std::runtime_error is thrown if an appropriate type is not available
uint32_t findAppropriateMemoryType(std::shared_ptr<PhysicalDevice> physicalDevice,
                                   uint32_t types,
                                   vk::MemoryPropertyFlags properties)
{
    vk::PhysicalDeviceMemoryProperties info = physicalDevice->getMemoryProperties();
    for (uint32_t i = 0; i < info.memoryTypeCount; ++i)
    {
        if ((types & (1 << i)) == 0)
            continue;
        if ((info.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }
    throw std::runtime_error("Vkx::findAppropriateMemoryType: failed to find an appropriate memory type");
}

//! This function creates a one-time command buffer and executes it. The function returns when the queue is idle.
//!
//! The function parameter should add commands as normal to the command buffer parameter, like this:
//! @code
//!     executeOnceSynced(device, commandPool, queue, [src, dst, size] (vk::CommandBuffer commands) {
//!         commands.copyBuffer(src, dst, vk::BufferCopy(0, 0, size));
//!     });
//! @endcode
//!
//! @param  device          The command buffer is allocated from this device
//! @param  commandPool     Commands are allocated from this pool
//! @param  queue           The command buffer is executed in this queue
//! @param  commands        Adds commands to the specified command buffer
//!
//! @note       commandPool should specify a pool optimized for transient command buffers

    void executeOnceSynched(std::shared_ptr<Device>            device,
                        vk::CommandPool const &                commandPool,
                        vk::Queue const &                      queue,
                        std::function<void(vk::CommandBuffer &)> commands)
{
    std::vector<vk::UniqueCommandBuffer> commandBuffers = device->allocateCommandBuffersUnique(
        vk::CommandBufferAllocateInfo(commandPool,
                                      vk::CommandBufferLevel::ePrimary,
                                      1));
    commandBuffers[0]->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    commands(commandBuffers[0].get());
    commandBuffers[0]->end();
    queue.submit(vk::SubmitInfo(0, nullptr, nullptr, 1, &commandBuffers[0].get()), nullptr);
    vkQueueWaitIdle(queue);
}
} // namespace Vkx
