#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace my_library {
    class TaskQueue {
    public:
        using Task = std::function<void()>;

        TaskQueue();
        ~TaskQueue();

        void enqueue(const Task & task);
        void enqueue2(Task task);
        void shutdown();

    private:
        void run();
        std::queue<Task> tasks_;
        std::mutex mutex_;
        std::condition_variable cv_;
        std::atomic<bool> shutdown_{false};
        std::thread thread_;
    };
} // namespace my_library