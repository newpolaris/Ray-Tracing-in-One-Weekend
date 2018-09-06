#include <gtest/gtest.h>
#include <Math/Random.h>
#include <iostream>

TEST(Random, srand) 
{
	for (int i = 0; i < 10000; i++)
	{
		auto k = size_t(Math::BaseRandom() * 2);
		EXPECT_LT(k, 2u);
	}
}
