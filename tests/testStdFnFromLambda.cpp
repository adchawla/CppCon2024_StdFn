#include "InstrumentedClass.h"
#include "MemoryManager.h"

#include <array>
#include <gtest/gtest.h>

TEST(StdFnFromLambda, LambdaCaptureByRef) {
    int x = 42;
    std::vector<int> v;
    v.reserve(10000);
    for (int i = 0; i < 10000; ++i) {
        v.push_back(i);
    }

    my_library::RecordMemoryManagerDelegate delegate;
    my_library::MemoryManager::setDelegate(&delegate);
    std::function<std::tuple<const int *, const int *>()> fn = [&]() {
        return std::make_tuple(&x, &v[0]);
    };
    my_library::MemoryManager::setDelegate(nullptr);

    auto [px, pv] = fn();

    EXPECT_EQ(px, &x);
    EXPECT_EQ(pv, &v[0]);
    EXPECT_EQ(delegate.allAllocations().size(), 0);
}

TEST(StdFnFromLambda, LambdaWithinSmallSizeOptimization) {
    int x = 42;
    std::vector<int> v;
    v.reserve(10000);
    for (int i = 0; i < 10000; ++i) {
        v.push_back(i);
    }
    auto oldAddress = &v[0];

    my_library::RecordMemoryManagerDelegate delegate;
    my_library::MemoryManager::setDelegate(&delegate);
    std::function<std::tuple<const int *, const int *>()> fn = [x = x, v = std::move(v)]() {
        return std::make_tuple(&x, &v[0]);
    };
    my_library::MemoryManager::setDelegate(nullptr);

    auto [px, pv] = fn();

    EXPECT_LE(sizeof(int) + sizeof(std::vector<int>), sizeof(fn));
    EXPECT_NE(px, &x);
    EXPECT_EQ(pv, oldAddress);
#if _DEBUG
    constexpr size_t expectedCount = 2; // std::vector on move does an allocation of 16 bytes.
#else
    constexpr size_t expectedCount = 0;
#endif
    EXPECT_EQ(delegate.allAllocations().size(), expectedCount);
}

TEST(StdFnFromLambda, OverSmallSizeOptimization) {
    constexpr auto nElements = sizeof(std::function<void()>) / sizeof(int);
    std::array<int, nElements> xs;
    std::vector<int> v;
    v.reserve(nElements);
    for (int i = 0; i < nElements; ++i) {
        v.push_back(i);
    }
    auto oldArrayAddress = &xs[0];
    const auto oldVectorAddress = &v[0];

    my_library::RecordMemoryManagerDelegate delegate;
    my_library::MemoryManager::setDelegate(&delegate);
    const std::function<std::tuple<const int *, const int *>()> fn = [xs = std::move(xs), v = std::move(v)]() {
        return std::make_tuple(&xs[0], &v[0]);
    };
    my_library::MemoryManager::setDelegate(nullptr);

    auto [px, pv] = fn();

    EXPECT_GT(sizeof(int[nElements]) + sizeof(std::vector<int>), sizeof(fn));
    EXPECT_NE(px, &xs[0]);
    EXPECT_EQ(pv, oldVectorAddress);
#if _DEBUG
    constexpr size_t expectedCount = 3; // std::vector on move does an allocation of 16 bytes.
#else
    constexpr size_t expectedCount = 1;
#endif
    EXPECT_EQ(delegate.allAllocations().size(), expectedCount);
};