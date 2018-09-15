#include <gtest/gtest.h>
#include <tools/string.h>

TEST(String, ReplaceAll) {
    using util::ReplaceAll;
    EXPECT_EQ(ReplaceAll("%f, %f, %f", "%f", "%.9g"), "%.9g, %.9g, %.9g");
}
