#include <gtest/gtest.h>
#include <Math/Bounds.h>
#include <iostream>

TEST(Bound2, iterator)
{
	Math::Bound2 b({0, 0}, {2, 5});
	int i = 0;
	for (auto p : b)
	{
		EXPECT_EQ(p, glm::ivec2(i % 2, i / 2));
		i++;
	}
}

// degenerate case from pbrt-v3
TEST(Bound2, iteratorDegenerate)
{
	using std::begin;
	using std::end;

	Math::Bound2 b0{{0, 0}, {0, 5}};
	EXPECT_EQ(begin(b0), end(b0));
	Math::Bound2 b1{{0, 0}, {2, 0}};
	EXPECT_EQ(begin(b1), end(b1));
	Math::Bound2 b2{{0, 0}, {0, 0}};
	EXPECT_EQ(begin(b2), end(b2));
}

