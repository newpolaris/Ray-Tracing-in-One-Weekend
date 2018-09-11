#include <gtest/gtest.h>
#include <tools/Parallel.h>
#include <iostream>
#include <atomic>

TEST(Parallel, loop1D)
{
    parallel::startup();

    std::atomic<int> counter(0);
    parallel::loop([&](int64_t) { ++counter; }, 10000);
    EXPECT_EQ(10000, counter);

    counter = 0;
    parallel::loop([&](int64_t) { ++counter; }, 20000);
    EXPECT_EQ(20000, counter);
    parallel::shutdown();

}

TEST(Parallel, loop2D)
{
    parallel::startup();

    std::atomic<int> counter(0);
    parallel::loop([&](glm::uvec2) { ++counter; }, glm::uvec2(11, 17));
    EXPECT_EQ(11*17, counter);

    parallel::shutdown();
}

TEST(Parallel, DoNothing) {
    parallel::startup();

    std::atomic<int> counter{0};
	parallel::loop([&](int64_t) { ++counter; }, 0);
    EXPECT_EQ(0, counter);

    counter = 0;
    parallel::loop([&](glm::uvec2 p) { ++counter; }, glm::uvec2(0, 0));
    EXPECT_EQ(0, counter);

    parallel::shutdown();
}
