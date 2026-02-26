#pragma once
#include "Texture.h"
#include <cstddef>

enum class GraphicsBackend
{
    DirectX11
};

namespace TextureLoader
{
    void Initialize(GraphicsBackend backend, void* device);
    bool IsInitialized();

    Texture LoadFromFile(const char* path);
    Texture LoadFromMemory(const void* data, size_t size);

    void Free(Texture& texture);
}
