#include "ConditionalStream.h"
#include "InstrumentedClass.h"
#include "utils.h"

#include <future>
#include <gtest/gtest.h>

using namespace my_library;
using namespace std;
using namespace my_test::utils;
TEST(CompareConstructs, testLambda) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

    [byValue = move(byValue), byRef = std::move(byRef), byCRef = move(byCRef)]() mutable {
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

#if WIN32
    async(launch::deferred, fn, move(byValue), std::move(byRef), std::move(byCRef)).get();
#else
    async(launch::deferred, fn2, move(byValue), move(byCRef)).get();
#endif
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

TEST(CompareConstructs, testApply) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");
    InstrumentedClass capturedInCb("capturedInCb");
    auto cbLambda = [capturedInCb = move(capturedInCb)](const auto & ids) {
        for (const auto & id : ids) {
            OSTREAM << id << ", ";
        }
        OSTREAM << capturedInCb.id() << endl;
    };

#if WIN32
    auto t =
        make_tuple(move(byValue), move(byRef), move(byCRef), std::move(cbLambda));
    apply(asyncFn, std::move(t));
#endif
}

TEST(CompareConstructs, testApply2) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byCRef("byCRef");
    InstrumentedClass capturedInCb("capturedInCb");
    auto cbLambda = [capturedInCb = move(capturedInCb)](const auto & ids) {
        for (const auto & id : ids) {
            OSTREAM << id << ", ";
        }
        OSTREAM << capturedInCb.id() << endl;
    };

    auto t = make_tuple(move(byValue), move(byCRef), std::move(cbLambda));
    apply(asyncFn2, std::move(t));
}
