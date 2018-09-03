#include "Parallel.h"
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <future>

namespace 
{
    using Task = std::function<void(void)>;

    bool bStop = false;
    std::mutex queue_mutex;
    std::queue<Task> tasks;
    std::vector<std::thread> workers;
    std::condition_variable cv;
}

void ParallelInit()
{
    for (int i = 0; i < 4; i++)
        workers.emplace_back(
            [&]{
                while (true)
                {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        cv.wait(lock, [&]{ return bStop || !tasks.empty(); });
                        if (bStop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
}

void ParallelEnd()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        bStop = true;
    }

    cv.notify_all();
    for (auto& t : workers)
        if (t.joinable())
            t.join();
}

void ParallelFor(std::function<void(int64_t)> function, int64_t count)
{
    ParallelInit();
    if (count < 1)
    {
        for (int64_t i = 0; i < count; i++)
            function(i);
        return;
    }

    for (int64_t i = 0; i < count; i++)
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace(std::bind(function, i));
        cv.notify_one();
    }
    ParallelEnd();
}
