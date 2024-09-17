#include <InstrumentedClass.h>
#include <algorithm>
#include <gtest/gtest.h>

auto getLambdaWithId(std::string id) {
    return [=](const my_library::InstrumentedClass & obj) {
        return obj.id() == id;
    };
};

TEST(LambdaUsage, FindIf) {
    std::vector<my_library::InstrumentedClass> v;
    v.reserve(5);
    for (const auto & id : {"a", "b", "c", "d", "e"}) {
        v.emplace_back(id);
    }

    // find the object with id "c"
    const auto it = std::find_if(v.begin(), v.end(), [&](const auto & obj) {
        return obj.id() == "c";
    });
    ASSERT_NE(it, v.end());
    ASSERT_EQ(it->id(), "c");
}

TEST(LambdaUsage, Sort) {
    std::vector<int> v;
    v.reserve(10);

    // Seed the random number generator
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Add 10 random numbers to the vector
    for (int i = 0; i < 10; ++i) {
        v.push_back(std::rand() % 100); // Random numbers between 0 and 99
    }

    // Sort the vector using a lambda function
    std::sort(v.begin(), v.end(), [](int a, int b) {
        return a < b;
    });

    // Verify that the vector is sorted
    for (size_t i = 1; i < v.size(); ++i) {
        ASSERT_LE(v[i - 1], v[i]);
    }
}