#include "Memory.h"

#ifdef _WIN32
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#define L1_CACHE_LINE_SIZE 64

void* AllocAligned(size_t size)
{
#ifdef _WIN32
    return _aligned_malloc(size, L1_CACHE_LINE_SIZE);
#else
	void* pointer = nullptr;
	if (posix_memalign(&pointer, L1_CACHE_LINE_SIZE, size) != 0)
		pointer = nullptr;
	return pointer;
#endif
}

void FreeAligned(void* pointer)
{
    if (pointer == nullptr)
        return;
#ifdef _WIN32
    _aligned_free(pointer);
#else
	free(pointer);
#endif
}
