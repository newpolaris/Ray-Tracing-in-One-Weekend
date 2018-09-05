#include <stdint.h>
#include <vector>

class MemoryArena
{
public:

	MemoryArena(size_t block_size = 256*1024) : block_size_(block_size)
	{
	}

	void* allocate(size_t requested) noexcept
	{
		return new char[requested];
	}

	template <typename T>
	T* allocate(size_t num, bool bExecuteConstructor = true) noexcept
	{
	}

	void deallocate(void*) noexcept {}

private:

	const size_t block_size_;
};

inline void* operator new(size_t size, MemoryArena& arena)
{
	return arena.allocate(size);
}

template <typename T>
void destroy(T* pointer, MemoryArena& arena)
{
	if (pointer)
	{
		pointer->~T();
		arena.deallocate(pointer);
	}
}
