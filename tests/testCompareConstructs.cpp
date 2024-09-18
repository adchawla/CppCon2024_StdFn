#include "InstrumentedClass.h"
#include "ConditionalStream.h"
#include "utils.h"

#include <gtest/gtest.h>
#include <future>

using namespace my_library;
using namespace std;
using namespace my_test::utils;
TEST(CompareConstructs, testLambda) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

    [byValue = move(byValue), byRef= std::move(byRef), byCRef = move(byCRef)] () mutable  {
        fn(move(byValue), byRef, byCRef);
    }();
}

TEST(CompareConstructs, testBind) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

    // bind can't directly work with function having reference parameters
    bind(fn2, move(byValue), move(byCRef))();
}

TEST(CompareConstructs, testAsync) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

    auto future = async(launch::deferred, fn, move(byValue), std::move(byRef), std::move(byCRef));
    future.get();
}

TEST(CompareConstructs, testAsync2) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

    auto future =
        async(launch::deferred, [byValue = move(byValue), byRef = std::move(byRef), byCRef = move(byCRef)]() mutable {
            fn(move(byValue), byRef, byCRef);
        });
    future.get();
}
