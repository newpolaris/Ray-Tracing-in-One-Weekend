#include "Parallel.h"
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <future>
#include <cassert>

namespace parallel
{
    using Task = std::function<void(void)>;

    thread_local int thread_index = 0;
    const int num_core = 4;

    bool bShutdown = false;
    std::mutex queue_mutex;
    std::queue<Task> tasks;
    std::vector<std::thread> workers;
    std::condition_variable cv;

    static void workerfunc(int index);
}

int parallel::count()
{
    return std::max(1u, std::thread::hardware_concurrency());
}

static void parallel::workerfunc(int index)
{
    thread_index = index;
    while (!bShutdown)
    {
        Task task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            cv.wait(lock, [&] { return bShutdown || !tasks.empty(); });
            if (tasks.empty()) break;
            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}

void parallel::startup()
{
    bShutdown = false;
    for (int i = 0; i < num_core; i++)
        workers.emplace_back(workerfunc, i+1);
}

void parallel::shutdown()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        bShutdown = true;
    }

    cv.notify_all();
    for (std::thread& t : workers) t.join();

    tasks.swap(std::queue<Task>());
    workers.clear();
}

// github.com/progschj/ThreadPool
std::future<void> parallel::enqueue(std::function<void(void)> function)
{
    auto task = std::make_shared<std::packaged_task<void()>>(function);
    auto future = task->get_future();
    std::unique_lock<std::mutex> lock(queue_mutex);
    tasks.emplace([task]() {(*task)();});
    cv.notify_one();
    return future;
}

void parallel::loop(std::function<void(int64_t)> function, int64_t count, int chunksize)
{
    if (count < chunksize || workers.empty())
    {
        for (int64_t i = 0; i < count; i++)
            function(i);
        return;
    }

    const size_t n = (size_t)(count / chunksize);
    std::vector<std::future<void>> futures;
    futures.reserve(n);
    for (size_t i = 0; i < n; i++)
    {
        const int64_t index_start = i * chunksize;
        const int64_t index_end = std::min(index_start + chunksize, count);
        futures.emplace_back(enqueue(
            [&, index_start, index_end]{
                for (int64_t k = index_start; k < index_end; k++)
                    function(k);
            }));
    }
    for (auto& f : futures) f.get();
}

void parallel::loop(std::function<void(glm::uvec2)> function, glm::uvec2 count)
{
    const int n = count.x * count.y;

    std::vector<std::future<void>> futures;
    futures.reserve(n);
    for (int i = 0; i < n; i++)
    {
        glm::uvec2 pt(n % count.x, n / count.x);
        futures.emplace_back(enqueue(std::bind(function, pt)));
    }
    for (auto& f : futures) f.get();
}