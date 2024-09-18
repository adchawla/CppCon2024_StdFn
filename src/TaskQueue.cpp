#include "TaskQueue.h"

#include <future>

namespace my_library {
    TaskQueue::TaskQueue() {
        thread_ = std::thread([this] {
            run();
        });
    }

    void TaskQueue::enqueue(const Task & task) {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.push(task);
        cv_.notify_one();
    }

    void TaskQueue::enqueue2(Task task) {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.push(std::move(task));
        cv_.notify_one();
    }

    void TaskQueue::shutdown() {
        shutdown_ = true;
        cv_.notify_one();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    void TaskQueue::waitForAllPreviousTasks() noexcept {
        std::promise<void> promise;
        enqueue2([&promise] {
            promise.set_value();
        });
        promise.get_future().wait();
    }

    void TaskQueue::run() {
        while (true) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] { return !tasks_.empty() || shutdown_; });
                if (shutdown_) {
                    break;
                }
                if (tasks_.empty()) {
                    continue;
                }
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            try {
                task();
            } catch (...) {
                // ignore
            }
        }
    }

    TaskQueue::~TaskQueue() {
        shutdown();
    }
}