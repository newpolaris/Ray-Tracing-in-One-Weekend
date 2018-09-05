#include <gtest/gtest.h>
#include <tools/MemoryArena.h>
#include <iostream>
#include <string>

class Temp
{
public:
	Temp(std::string name) : name_(std::move(name)) {}
	std::string print() const { return name_; }
	std::string name_;
};

TEST(MemoryArena, placement) 
{
	MemoryArena arena;
	void* data = arena.allocate(100);
	auto temp0 = new (data) Temp("Hello");
	EXPECT_EQ(temp0->print(), "Hello");

	auto temp1 = new (arena) Temp("World");
	EXPECT_EQ(temp1->print(), "World");
	destroy(temp1, arena);
}
