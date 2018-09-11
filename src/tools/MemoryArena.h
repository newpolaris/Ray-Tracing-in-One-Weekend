#pragma once

#include <stdint.h>
#include <vector>
#include <tools/Memory.h>

class MemoryArena
{
public:

	MemoryArena(size_t block_size = 256*1024) 
        : block_size_(block_size)
	{
	}

    ~MemoryArena() 
    {
        for (auto& b : used_large_block_) delete[] b;
        for (auto& b : used_block_) delete[] b;
        used_large_block_.clear();
        used_block_.clear();
    }

    void request_block() noexcept
    {
        if (free_block_.size() > 0)
        {
            current_pointer_ = free_block_.back();
            current_allocsize_ = 0;
            used_block_.push_back(current_pointer_);
            free_block_.pop_back();
        }
        else 
        {
            current_allocsize_ = 0;
            current_pointer_ = new uint8_t[block_size_];
            used_block_.push_back(current_pointer_);
        }
    }

    void clear() 
    {
        for (auto& b : used_large_block_) delete[] b;
        used_large_block_.clear();

        free_block_.insert(std::end(free_block_), std::begin(used_block_), std::end(used_block_));
        used_block_.clear();
        current_allocsize_ = 0;
        current_pointer_ = nullptr;
    }

    uint8_t* create_large_block(size_t size) noexcept
    {
        return new uint8_t[size];
    }

	void* allocate(size_t requested) noexcept
	{
        const int align = alignof(std::max_align_t);
        const size_t nBytes = (requested + align - 1) & ~(align - 1);
        if (nBytes > block_size_)
        {
            uint8_t* data = create_large_block(nBytes);
            used_large_block_.push_back(data);
            return data;
        }

        if (current_pointer_ == nullptr 
         || current_allocsize_ + nBytes > block_size_)
            request_block();
        uint8_t* data = current_pointer_;
        current_pointer_ += nBytes;
        current_allocsize_ += nBytes;
        return data;
	}

	template <typename T, typename ...Args>
	T* allocate(size_t num, Args&&... args) noexcept
	{
        T* mem = (T*)allocate(num * sizeof(T));
        for (size_t i = 0; i < num; i++)
            new (&mem[i]) T(std::forward<Args>(args)...);

        return mem;
	}

	void deallocate(void*) noexcept {}

private:

    MemoryArena(const MemoryArena&) = delete;
    MemoryArena& operator=(const MemoryArena&) = delete;

	const size_t block_size_;
    size_t current_allocsize_;
    uint8_t* current_pointer_;
    std::vector<uint8_t*> free_block_, used_block_;
    std::vector<uint8_t*> used_large_block_;
};

inline void* operator new(size_t size, MemoryArena& arena) noexcept
{
	return arena.allocate(size);
}

inline void operator delete(void* pointer, MemoryArena& arena) noexcept
{
    return arena.deallocate(pointer);
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
