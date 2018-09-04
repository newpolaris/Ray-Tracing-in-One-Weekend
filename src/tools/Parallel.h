#include <functional>
#include <future>
#include <glm/glm.hpp>

namespace parallel
{
    void startup();
    void shutdown();
    void loop(std::function<void(int64_t)> function, int64_t count, int chunksize = 1);
    void loop(std::function<void(glm::uvec2)> function, glm::uvec2 count);
    std::future<void> enqueue(std::function<void(void)> function);
}
