#if !defined(VKX_VKX_H)
#define VKX_VKX_H

#pragma once

#include <functional>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

//! Extensions to Vulkan.
namespace Vkx
{
class Buffer;
class Device;
class PhysicalDevice;

class Material
{
};

//! Returns true if the extension is supported.
bool extensionIsSupported(std::vector<vk::ExtensionProperties> & extensions, char const * name);

//! Returns true if all extensions are supported by the device.
bool allExtensionsSupported(vk::PhysicalDevice const &        physicalDevice,
                            std::vector<char const *> const & extensions);

//! Returns true if the layer is available.
bool layerIsAvailable(std::vector<vk::LayerProperties> layers, char const * name);

//! Returns true if all of the specified extensions are available.
bool allLayersAvailable(std::vector<char const *> const & requested);

//! Loads a shader.
vk::ShaderModule loadShaderModule(std::string const &         path,
                                  std::shared_ptr<Device>     device,
                                  vk::ShaderModuleCreateFlags flags = vk::ShaderModuleCreateFlags());

//! Finds an appropriate memory type provided by the physical device.
uint32_t findAppropriateMemoryType(std::shared_ptr<PhysicalDevice> physicalDevice,
                                   uint32_t                        types,
                                   vk::MemoryPropertyFlags         properties);

//! Creates and executes a one-time command buffer.
void executeOnceSynched(std::shared_ptr<Device>                  device,
                        vk::CommandPool const &                  commandPool,
                        vk::Queue const &                        queue,
                        std::function<void(vk::CommandBuffer &)> commands);

//! Strips a grid generating 16-bit indexes.
int stripGrid(int w, int h, uint16_t * pData);

//! Strips a grid generating 32-bit indexes.
int stripGrid(int w, int h, uint32_t * pData);

namespace ColorComponentFlags
{
    //! All colors
    vk::ColorComponentFlags const all = vk::ColorComponentFlags(vk::FlagTraits<vk::ColorComponentFlagBits>::allFlags);
}

namespace DebugUtils
{
namespace MessageTypeFlags
{
    //! All message types
    vk::DebugUtilsMessageTypeFlagsEXT const all = vk::DebugUtilsMessageTypeFlagsEXT(
        vk::FlagTraits<vk::DebugUtilsMessageTypeFlagBitsEXT>::allFlags);
}

namespace MessageSeverityFlags
{
    //! All message severities
    vk::DebugUtilsMessageSeverityFlagsEXT const all = vk::DebugUtilsMessageSeverityFlagsEXT(
        vk::FlagTraits<vk::DebugUtilsMessageSeverityFlagBitsEXT>::allFlags);                                                                                     //!<
    //!< All
    //!< severities
}
}   // namespace DebugUtils
} // namespace Vkx

#endif // !defined(VKX_VKX_H)
