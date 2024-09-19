#include "ConditionalStream.h"
#include "InstrumentedClass.h"
#include "utils.h"

#include <future>
#include <gtest/gtest.h>

using namespace my_library;
using namespace my_test::utils;

TEST(CompareConstructs, testLambda) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

    [byValue = std::move(byValue), byRef = std::move(byRef), byCRef = std::move(byCRef)]() mutable {
        fn(std::move(byValue), byRef, byCRef);
    }();
}

TEST(CompareConstructs, testBind) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

    // bind can't directly work with function having reference parameters
    std::bind(fn2, std::move(byValue), std::move(byCRef))();
}

TEST(CompareConstructs, testAsync) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

#if WIN32
    std::async(std::launch::deferred, fn, std::move(byValue), std::move(byRef), std::move(byCRef)).get();
#else
    async(std::launch::deferred, fn2, std::move(byValue), std::move(byCRef)).get();
#endif
}

TEST(CompareConstructs, testAsync2) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

    auto future = std::async(
        std::launch::deferred,
        [byValue = std::move(byValue), byRef = std::move(byRef), byCRef = std::move(byCRef)]() mutable {
            fn(std::move(byValue), byRef, byCRef);
        });
    future.get();
}

TEST(CompareConstructs, testApply) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");
    InstrumentedClass capturedInCb("capturedInCb");
    auto cbLambda = [capturedInCb = std::move(capturedInCb)](const auto & ids) {
        for (const auto & id : ids) {
            OSTREAM << id << ", ";
        }
        OSTREAM << capturedInCb.id() << std::endl;
    };

#if WIN32
    auto t = std::make_tuple(std::move(byValue), std::move(byRef), std::move(byCRef), std::move(cbLambda));
    apply(asyncFn, std::move(t));
#endif
}

TEST(CompareConstructs, testApply2) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byCRef("byCRef");
    InstrumentedClass capturedInCb("capturedInCb");
    auto cbLambda = [capturedInCb = std::move(capturedInCb)](const auto & ids) {
        for (const auto & id : ids) {
            OSTREAM << id << ", ";
        }
        OSTREAM << capturedInCb.id() << std::endl;
    };

    auto t = std::make_tuple(std::move(byValue), std::move(byCRef), std::move(cbLambda));
    apply(asyncFn2, std::move(t));
}
