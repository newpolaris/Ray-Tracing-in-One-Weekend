#include <functional>

void ParallelFor(std::function<void(int64_t)> function, int64_t count);