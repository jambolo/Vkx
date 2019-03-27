#if !defined(VKX_TEXTUREMANAGER_H)
#define VKX_TEXTUREMANAGER_H

#pragma once

#include <Vkx/Image.h>

namespace Vkx
{
//! An abstract class that loads and manages textures.
    class TextureManager
    {
public:
        TextureManager()          = default;
        virtual ~TextureManager() = default;
    };
} // namespace Vkx

#endif // !defined(VKX_TEXTUREMANAGER_H)
