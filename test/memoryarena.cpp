#include <gtest/gtest.h>
#include <tools/MemoryArena.h>
#include <tools/Allocator.h>
#include <iostream>
#include <memory>
#include <string>

class Temp
{
public:
	Temp(std::string name) : name_(std::move(name)) {}
    ~Temp() {}
	std::string print() const { return name_; }
	std::string name_;
};

TEST(MemoryArena, placement) 
{
	MemoryArena arena;
	void* data = arena.allocate(100);
	auto temp0 = new (data) Temp("Hello");
	EXPECT_EQ(temp0->print(), "Hello");
    destroy(temp0, arena);

	auto temp1 = new (arena) Temp("World");
	EXPECT_EQ(temp1->print(), "World");
	destroy(temp1, arena);
}

TEST(MemoryArena, allocator)
{
    MemoryArena arena;
    AreaAllocator<int> alloc(&arena);
    std::shared_ptr<int> foo = std::allocate_shared<int>(alloc, 10);
    EXPECT_EQ(*foo, 10);

    std::vector<int, AreaAllocator<int>> hello(10, 1, alloc);
    EXPECT_EQ(hello[5], 1);

    std::shared_ptr<Temp> foo2 = std::allocate_shared<Temp>(alloc, "Hello");
	EXPECT_EQ(foo2->print(), "Hello");

    AreaAllocator<Temp> allocT(&arena);
    std::vector<int, AreaAllocator<int>> hello2(10, 1, allocT);
}