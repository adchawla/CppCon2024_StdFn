#include "ConditionalStream.h"
#include "InstrumentedClass.h"
#include "MemoryManager.h"

#include <array>
#include <gtest/gtest.h>

using namespace my_library;
TEST(Lambda, DefaultSize) {
    auto lambda = []() {
    };

    EXPECT_EQ(1, sizeof(lambda));
}

TEST(Lambda, CaptureByRef) {
    // assume it is a 64-bit machine and sizeof(InstrumentedClass) == 32
    InstrumentedClass obj1{"a"};
    InstrumentedClass obj2{"b"};

    auto lambda = [&] {
        return std::make_tuple(obj1.id(), obj2.id());
    };

    auto [id1, id2] = lambda();

    OSTREAM << "(" << id1 << ", " << id2 << ")" << std::endl;
    OSTREAM << "sizeof(lambda) = " << sizeof(lambda) << std::endl;

    EXPECT_EQ(sizeof(lambda), 2 * sizeof(void *));

    EXPECT_EQ(id1, "a");
    EXPECT_EQ(id2, "b");
}

TEST(Lambda, CaptureByMove) {
    int x = 42;
    std::vector<int> v;
    v.reserve(1024);
    for (int i = 0; i < 1024; ++i) {
        v.push_back(i);
    }
    const auto oldAddress = &v[0];

    my_library::RecordMemoryManagerDelegate delegate;
    my_library::MemoryManager::setDelegate(&delegate);
    auto lambda = [x = std::move(x), v = std::move(v)] {
        return std::make_tuple(&x, v.data());
    };
    my_library::MemoryManager::setDelegate(nullptr);

    auto [px, ps] = lambda();

    struct Temp {
        int x;
        std::vector<int> v;
    };

    EXPECT_EQ(sizeof(lambda), sizeof(Temp));

    EXPECT_NE(px, &x);
    EXPECT_EQ(ps, oldAddress);
#if _DEBUG
    constexpr size_t expectedCount = 1; // std::vector on move does an allocation of 16 bytes.
#else
    constexpr size_t expectedCount = 0;
#endif

    EXPECT_EQ(expectedCount, delegate.allAllocations().size());
}

TEST(Lambda, CaptureByValue) {
    // assume it is a 64-bit machine and sizeof(InstrumentedClass) == 32
    InstrumentedClass obj1{"a"};
    InstrumentedClass obj2{"b"};

    auto lambda = [=] {
        return std::make_tuple(obj1.id(), obj2.id());
    };

    auto [id1, id2] = lambda();

    OSTREAM << "(" << id1 << ", " << id2 << ")" << std::endl;
    OSTREAM << "sizeof(lambda) = " << sizeof(lambda) << std::endl;

    EXPECT_EQ(sizeof(lambda), 2 * sizeof(InstrumentedClass));

    EXPECT_EQ(id1, "C(a)");
    EXPECT_EQ(id2, "C(b)");
}
