#include "Memory.h"
#include <malloc.h>

#define L1_CACHE_LINE_SIZE 64

void* AllocAligned(size_t size)
{
    return _aligned_malloc(size, L1_CACHE_LINE_SIZE);
}

void FreeAligned(void* pointer)
{
    if (pointer == nullptr)
        return;
    _aligned_free(pointer);
}
