#include <gtest/gtest.h>
#include <Math/Common.h>
#include <iostream>

TEST(Common, Log2) 
{
	EXPECT_EQ(2, Math::Log2((int32_t)7));
	EXPECT_EQ(5, Math::Log2((int32_t)32));
	EXPECT_EQ(62, Math::Log2((int64_t)1 << 62));
}
