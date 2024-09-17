#include "InstrumentedClass.h"
#include "TaskQueue.h"

#include <array>
#include <future>
#include <gtest/gtest.h>

using namespace my_library;

TEST(TaskQueue, enqueue) {
    TaskQueue taskQueue;
    std::promise<bool> promise;
    taskQueue.enqueue([&] {
        promise.set_value(true);
    });
    EXPECT_EQ(true, promise.get_future().get());
}

TEST(TaskQueue, enqueue2) {
    TaskQueue taskQueue;
    std::promise<bool> promise;
    taskQueue.enqueue2([&] {
        promise.set_value(true);
    });
    EXPECT_EQ(true, promise.get_future().get());
}

TEST(TaskQueue, LambdaCaptureByCopy) {
    TaskQueue taskQueue;
    std::promise<std::string> promise;
    InstrumentedClass obj("a");
    constexpr auto nElements = sizeof(std::function<void()>) / sizeof(int);
    std::array<int, nElements> xs;
    
    taskQueue.enqueue([&promise, obj, xs] {
        std::cout << xs[0];
        promise.set_value(obj.id());
    });
    EXPECT_EQ("M(M(M(C(a))))", promise.get_future().get());
}

TEST(TaskQueue, LambdaCaptureByMove) {
    TaskQueue taskQueue;
    std::promise<std::string> promise;
    InstrumentedClass obj("a");
    constexpr auto nElements = sizeof(std::function<void()>) / sizeof(int);
    std::array<int, nElements> xs;

    taskQueue.enqueue([&promise, obj = std::move(obj), xs] {
        std::cout << xs[0];
        promise.set_value(obj.id());
    });
    EXPECT_EQ("M(M(M(M(a))))", promise.get_future().get());
}