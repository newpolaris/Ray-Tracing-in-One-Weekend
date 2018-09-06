#pragma once

#include <cassert>
#include <memory>
#include "Memory.h"
#include "MemoryArena.h"

template<typename T>
struct MaxAllocations
{
  enum {value = static_cast<std::size_t>(-1) / sizeof(T)};
};

template <typename T>
struct AreaAllocator
{
    template<typename U>
    friend struct AreaAllocator;

public:

    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template <typename U>
    struct rebind { typedef AreaAllocator<U> other; };

    explicit AreaAllocator(MemoryArena* arena) : arena_(arena) {}
    AreaAllocator(const AreaAllocator& alloc) throw()
    {
        arena_ = alloc.arena_;
    }

    template <typename U>
    AreaAllocator(const AreaAllocator<U>& other) throw() 
    {
        arena_ = other.arena_;
    } 

    ~AreaAllocator()
    {
        arena_ = nullptr;
    }

    AreaAllocator& operator=(const AreaAllocator& alloc)
    {
        arena_ = alloc.arena_;
        return *this;
    }

    template <typename U>
    AreaAllocator& operator=(const AreaAllocator<U>& alloc)
    {
        arena_ = alloc.arena_;
        return *this;
    }

    pointer address(reference x) const
    {
        return &x;
    }

    const_pointer address(const_reference x) const
    {
        return &x;
    }

    T* allocate(std::size_t n, const void* hint = 0)
    {
        assert(arena_ != nullptr);
        return (T*)arena_->allocate(n * sizeof(T));
    }

    void deallocate(T* ptr, size_type n)
    {
        assert(arena_ != nullptr);
        arena_->deallocate(ptr);
    }

    void construct(pointer p, const T& val)
    {
        new ((T*)p) T(val);
    }

    template<typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        new ((void *)p) U(std::forward<Args>(args)...);
    }
 
    void destroy(pointer p)
    {
        p->~T();
    }

    template<typename U>
    void destroy(U* p)
    {
        p->~U();
    }

    bool operator==(const AreaAllocator& alloc) const
    {
        return arena_ == alloc.arena_;
    }

    size_type max_size(void) const { return MaxAllocations<T>::value; }

private:

    MemoryArena* arena_;
};

template <typename T, typename U>
inline bool operator==(const AreaAllocator<T>& a, const AreaAllocator<U>& b)
{
    return a.operator==(b);
}

template <typename T, typename U>
inline bool operator != (const AreaAllocator<T>& a, const AreaAllocator<U>& b)
{
    return !(a == b);
}

typedef AreaAllocator<uint8_t> AreaAlloc;