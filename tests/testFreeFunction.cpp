#include "ConditionalStream.h"
#include "InstrumentedClass.h"
#include "TaskQueue.h"
#include "utils.h"

#include <future>
#include <gtest/gtest.h>

using namespace my_library;
using namespace my_test::utils;

TEST(FreeFunction, DirectCall) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

    std::vector<std::string> ids;

    asyncFn(byValue, byRef, byCRef, [&ids](auto strings) mutable {
        ids = std::move(strings);
    });

    EXPECT_EQ(ids[0], "C(byValue)");
    EXPECT_EQ(ids[1], "byRef");
    EXPECT_EQ(ids[2], "byCRef");
}

TEST(FreeFunction, EnqueueOnTaskQueueNoOptimizationsInPlace) {
    TaskQueue taskQueue;

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

    taskQueue.enqueue([byValue = std::move(byValue), byRef = std::move(byRef), byCRef = std::move(byCRef),
                       callbackFn = std::move(cbLambda)]() mutable {
        asyncFn(byValue, byRef, byCRef, callbackFn);
    });

    taskQueue.waitForAllPreviousTasks();
    taskQueue.shutdown();
}

TEST(FreeFunction, EnqueueOnTaskQueueOptimizationsInPlace) {
    TaskQueue taskQueue;

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

    taskQueue.enqueue([byValue = std::move(byValue), byRef = std::move(byRef), byCRef = std::move(byCRef),
                       callbackFn = std::move(cbLambda)]() mutable {
        asyncFn(std::move(byValue), byRef, byCRef, std::move(callbackFn));
    });
    taskQueue.waitForAllPreviousTasks();

    taskQueue.shutdown();
}

TEST(FreeFunction, Enqueue2OnTaskQueue) {
    TaskQueue taskQueue;

    taskQueue.enqueue2(
        [byValue = InstrumentedClass("byValue"), byRef = InstrumentedClass("byRef"),
         byCRef = InstrumentedClass("byCRef"),
         callbackFn = std::function([capturedInCb = InstrumentedClass("capturedInCb")](std::vector<std::string> ids) {
             OSTREAM << "ids.size() = " << ids.size() << std::endl;
             for (const auto & id : ids) {
                 OSTREAM << id << std::endl;
             }
             OSTREAM << "capturedInCb.id() = " << capturedInCb.id() << std::endl;
         })]() mutable {
            asyncFn(std::move(byValue), byRef, byCRef, callbackFn);
        });
    taskQueue.waitForAllPreviousTasks();

    taskQueue.shutdown();
}
