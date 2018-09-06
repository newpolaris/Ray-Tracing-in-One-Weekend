#pragma once

#include <memory>
#include "Memory.h"
#include "MemoryArena.h"

template <typename T>
struct AreaAllocator
{
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template <typename U>
    struct rebind { typedef AreaAllocator<U> other; };

    AreaAllocator() throw() {}

    template <typename U>
    AreaAllocator(const AreaAllocator<U>& other) noexcept {} 

    T* allocate(std::size_t n, const void* hint = 0)
    {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* ptr, size_type n)
    {
        ::operator delete(ptr);
    }
};

template <typename T, typename U>
inline bool operator == (const AreaAllocator<T>&, const AreaAllocator<U>&)
{
    return true;
}

template <typename T, typename U>
inline bool operator != (const AreaAllocator<T>& a, const AreaAllocator<U>& b)
{
    return !(a == b);
}