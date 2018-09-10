#pragma once

#include <cstddef>

// use code from [PBRT]
void* AllocAligned(size_t size);
void FreeAligned(void* pointer);

template <typename T>
T* AllocAligned(size_t count)
{
    return (T*)AllocAligned(count * sizeof(T));
}
