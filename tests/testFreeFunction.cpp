#include "ConditionalStream.h"
#include "InstrumentedClass.h"
#include "TaskQueue.h"

#include <future>
#include <gtest/gtest.h>

using namespace my_library;

using CallbackFn = std::function<void(std::vector<std::string>)>;

void asyncFn(
    InstrumentedClass byValue, InstrumentedClass & byRef, const InstrumentedClass & byCRef,
    const std::function<void(std::vector<std::string>)> & callbackFn) {
    callbackFn(std::vector{byValue.id(), byRef.id(), byCRef.id()});
}

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

TEST(FreeFunction, EnqueueOnTaskQueue) {
    std::promise<std::vector<std::string>> promise;
    TaskQueue taskQueue;

    taskQueue.enqueue([byValue = InstrumentedClass("byValue"), byRef = InstrumentedClass("byRef"),
                       byCRef = InstrumentedClass("byCRef"),
                       callbackFn = [&promise, capturedInCb = InstrumentedClass("capturedInCb")](const auto & ids) {
                           for (const auto & id : ids) {
                               OSTREAM << id << ", ";
                           }
                           OSTREAM << capturedInCb.id() << std::endl;
                           std::vector<std::string> idsCopy(ids);
                           idsCopy.push_back(capturedInCb.id());
                           promise.set_value(std::move(idsCopy));
                       }]() mutable {
        asyncFn(std::move(byValue), byRef, byCRef, callbackFn);
    });

    const auto ids = promise.get_future().get();

    taskQueue.shutdown();
}

TEST(FreeFunction, Enqueue2OnTaskQueue) {
    std::promise<std::vector<std::string>> promise;
    TaskQueue taskQueue;

    taskQueue.enqueue2(
        [byValue = InstrumentedClass("byValue"), byRef = InstrumentedClass("byRef"),
         byCRef = InstrumentedClass("byCRef"),
         callbackFn = [&promise, capturedInCb = InstrumentedClass("capturedInCb")](std::vector<std::string> ids) {
             OSTREAM << "ids.size() = " << ids.size() << std::endl;
             for (const auto & id : ids) {
                 OSTREAM << id << std::endl;
             }
             OSTREAM << "capturedInCb.id() = " << capturedInCb.id() << std::endl;
             ids.push_back(capturedInCb.id());
             promise.set_value(std::move(ids));
         }]() mutable {
            asyncFn(std::move(byValue), byRef, byCRef, callbackFn);
        });

    const auto ids = promise.get_future().get();

    taskQueue.shutdown();
}

#if 0
TEST(FreeFunction, ExecuteOnTaskQueue) {
    TaskQueue taskQueue;
    std::promise<std::tuple<std::string, std::string, std::string>> promise;

    taskQueue.enqueue([&promise] {
        InstrumentedClass byValue("byValue");
        InstrumentedClass byRef("byRef");
        InstrumentedClass byCRef("byCRef");

        asyncFn(
            byValue, byRef, byCRef,
            [&promise, ](
                const InstrumentedClass & byValue, const InstrumentedClass & byRef, const InstrumentedClass & byCRef) {
                promise.set_value(std::make_tuple(byValue.id(), byRef.id(), byCRef.id()));
            });
    });

    auto [idValue, idRef, idCRef] = promise.get_future().get();
    EXPECT_EQ(idValue, "C(byValue)");
    EXPECT_EQ(idRef, "byRef");
    EXPECT_EQ(idCRef, "byCRef");

    taskQueue.shutdown();
}

TEST(FreeFunction, ExecuteOnTaskQueueUsingLambda) {
    TaskQueue taskQueue;
    std::promise<std::tuple<std::string, std::string, std::string>> promise;

    taskQueue.enqueue([&promise, byValue = InstrumentedClass("byValue"), byRef = InstrumentedClass("byRef"),
                       byCRef = InstrumentedClass("byCRef")]() mutable {
        freeAsyncFn(
            byValue, byRef, byCRef,
            [&promise](
                const InstrumentedClass & byValue, const InstrumentedClass & byRef, const InstrumentedClass & byCRef) {
                promise.set_value(std::make_tuple(byValue.id(), byRef.id(), byCRef.id()));
            });
    });

    auto [idValue, idRef, idCRef] = promise.get_future().get();
    EXPECT_EQ(idValue, "C(M(M(byValue)))");
    EXPECT_EQ(idRef, "M(M(byRef))");
    EXPECT_EQ(idCRef, "M(M(byCRef))");

    taskQueue.shutdown();
}

TEST(FreeFunction, ExecuteOnTaskQueueUsingBind) {
    TaskQueue taskQueue;
    std::promise<std::tuple<std::string, std::string, std::string>> promise;

    taskQueue.enqueue(std::bind(
        [&promise](InstrumentedClass byValue, InstrumentedClass & byRef, const InstrumentedClass & byCRef) {
            freeAsyncFn(
                byValue, byRef, byCRef,
                [&promise](
                    const InstrumentedClass & byValue, const InstrumentedClass & byRef,
                    const InstrumentedClass & byCRef) {
                    promise.set_value(std::make_tuple(byValue.id(), byRef.id(), byCRef.id()));
                });
        },
        InstrumentedClass("byValue"), InstrumentedClass("byRef"), InstrumentedClass("byCRef")));

    auto [idValue, idRef, idCRef] = promise.get_future().get();

    EXPECT_EQ(idValue, "C(C(M(M(byValue))))");
    EXPECT_EQ(idRef, "M(M(byRef))");
    EXPECT_EQ(idCRef, "M(M(byCRef))");

    taskQueue.shutdown();
}

TEST(FreeFunction, ExecuteOnThreadUsingAsync) {
    std::promise<std::tuple<std::string, std::string, std::string>> promise;

    auto future = std::async(
        std::launch::async, freeAsyncFn, InstrumentedClass("byValue"), InstrumentedClass("byRef"),
        InstrumentedClass("byCRef"),
        [&promise](
            const InstrumentedClass & byValue, const InstrumentedClass & byRef, const InstrumentedClass & byCRef) {
            promise.set_value(std::make_tuple(byValue.id(), byRef.id(), byCRef.id()));
        });

    auto [idValue, idRef, idCRef] = promise.get_future().get();

    EXPECT_EQ(idValue, "M(M(M(byValue)))");
    EXPECT_EQ(idRef, "M(M(byRef))");
    EXPECT_EQ(idCRef, "M(M(byCRef))");
}

TEST(FreeFunction, ExecuteOnTaskQueueUsingLambda2Levels) {
    TaskQueue taskQueue;
    std::promise<std::tuple<std::string, std::string, std::string>> promise;

    taskQueue.enqueue([&promise, &taskQueue, byValue = InstrumentedClass("byValue"), byRef = InstrumentedClass("byRef"),
                       byCRef = InstrumentedClass("byCRef")]() {
        taskQueue.enqueue(
            [&promise, byValue = std::move(byValue), byRef = std::move(byRef), byCRef = std::move(byCRef)]() mutable {
                freeAsyncFn(
                    byValue, byRef, byCRef,
                    [&promise](
                        const InstrumentedClass & byValue, const InstrumentedClass & byRef,
                        const InstrumentedClass & byCRef) {
                        promise.set_value(std::make_tuple(byValue.id(), byRef.id(), byCRef.id()));
                    });
            });
    });

    auto [idValue, idRef, idCRef] = promise.get_future().get();
    EXPECT_EQ(idValue, "C(M(C(M(M(byValue)))))");
    EXPECT_EQ(idRef, "M(C(M(M(byRef))))");
    EXPECT_EQ(idCRef, "M(C(M(M(byCRef))))");

    taskQueue.shutdown();
}
#endif