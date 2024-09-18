#include "ConditionalStream.h"
#include "InstrumentedClass.h"
#include "TaskQueue.h"

#include <future>
#include <gtest/gtest.h>

using namespace my_library;
using namespace std;

using CallbackFn = function<void(vector<string>)>;

void asyncFn(
    InstrumentedClass byValue, InstrumentedClass & byRef, const InstrumentedClass & byCRef,
    const function<void(vector<string>)> & callbackFn) {
    callbackFn(vector{byValue.id(), byRef.id(), byCRef.id()});
}

TEST(FreeFunction, DirectCall) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

    vector<string> ids;

    asyncFn(byValue, byRef, byCRef, [&ids](auto strings) mutable {
        ids = move(strings);
    });

    EXPECT_EQ(ids[0], "C(byValue)");
    EXPECT_EQ(ids[1], "byRef");
    EXPECT_EQ(ids[2], "byCRef");
}

TEST(FreeFunction, EnqueueOnTaskQueueNoOptimizationsInPlace) {
    promise<vector<string>> promise;
    TaskQueue taskQueue;

    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");
    InstrumentedClass capturedInCb("capturedInCb");

    auto cbLambda = [&promise, capturedInCb = move(capturedInCb)](const auto & ids) {
        for (const auto & id : ids) {
            OSTREAM << id << ", ";
        }
        OSTREAM << capturedInCb.id() << endl;
        auto idsCopy{ids};
        idsCopy.push_back(capturedInCb.id());
        promise.set_value(move(idsCopy));
    };

    taskQueue.enqueue(
        [byValue = move(byValue), byRef = move(byRef), byCRef = move(byCRef), callbackFn = move(cbLambda)]() mutable {
            asyncFn(byValue, byRef, byCRef, callbackFn);
        });
    const auto ids = promise.get_future().get();

    taskQueue.shutdown();
}

TEST(FreeFunction, EnqueueOnTaskQueueOptimizationsInPlace) {
    promise<vector<string>> promise;
    TaskQueue taskQueue;

    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");
    InstrumentedClass capturedInCb("capturedInCb");

    auto cbLambda = [&promise, capturedInCb = move(capturedInCb)](const auto & ids) {
        for (const auto & id : ids) {
            OSTREAM << id << ", ";
        }
        OSTREAM << capturedInCb.id() << endl;
        auto idsCopy{ids};
        idsCopy.push_back(capturedInCb.id());
        promise.set_value(move(idsCopy));
    };

    taskQueue.enqueue(
        [byValue = move(byValue), byRef = move(byRef), byCRef = move(byCRef), callbackFn = move(cbLambda)]() mutable {
            asyncFn(move(byValue), byRef, byCRef, move(callbackFn));
        });
    const auto ids = promise.get_future().get();

    taskQueue.shutdown();
}

TEST(FreeFunction, Enqueue2OnTaskQueue) {
    promise<vector<string>> promise;
    TaskQueue taskQueue;

    taskQueue.enqueue2(
        [byValue = InstrumentedClass("byValue"), byRef = InstrumentedClass("byRef"),
         byCRef = InstrumentedClass("byCRef"),
         callbackFn = function([&promise, capturedInCb = InstrumentedClass("capturedInCb")](vector<string> ids) {
             OSTREAM << "ids.size() = " << ids.size() << endl;
             for (const auto & id : ids) {
                 OSTREAM << id << endl;
             }
             OSTREAM << "capturedInCb.id() = " << capturedInCb.id() << endl;
             ids.push_back(capturedInCb.id());
             promise.set_value(move(ids));
         })]() mutable {
            asyncFn(move(byValue), byRef, byCRef, callbackFn);
        });

    const auto ids = promise.get_future().get();

    taskQueue.shutdown();
}

#if 0
TEST(FreeFunction, ExecuteOnTaskQueue) {
    TaskQueue taskQueue;
    promise<tuple<string, string, string>> promise;

    taskQueue.enqueue([&promise] {
        InstrumentedClass byValue("byValue");
        InstrumentedClass byRef("byRef");
        InstrumentedClass byCRef("byCRef");

        asyncFn(
            byValue, byRef, byCRef,
            [&promise, ](
                const InstrumentedClass & byValue, const InstrumentedClass & byRef, const InstrumentedClass & byCRef) {
                promise.set_value(make_tuple(byValue.id(), byRef.id(), byCRef.id()));
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
    promise<tuple<string, string, string>> promise;

    taskQueue.enqueue([&promise, byValue = InstrumentedClass("byValue"), byRef = InstrumentedClass("byRef"),
                       byCRef = InstrumentedClass("byCRef")]() mutable {
        freeAsyncFn(
            byValue, byRef, byCRef,
            [&promise](
                const InstrumentedClass & byValue, const InstrumentedClass & byRef, const InstrumentedClass & byCRef) {
                promise.set_value(make_tuple(byValue.id(), byRef.id(), byCRef.id()));
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
    promise<tuple<string, string, string>> promise;

    taskQueue.enqueue(bind(
        [&promise](InstrumentedClass byValue, InstrumentedClass & byRef, const InstrumentedClass & byCRef) {
            freeAsyncFn(
                byValue, byRef, byCRef,
                [&promise](
                    const InstrumentedClass & byValue, const InstrumentedClass & byRef,
                    const InstrumentedClass & byCRef) {
                    promise.set_value(make_tuple(byValue.id(), byRef.id(), byCRef.id()));
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
    promise<tuple<string, string, string>> promise;

    auto future = async(
        launch::async, freeAsyncFn, InstrumentedClass("byValue"), InstrumentedClass("byRef"),
        InstrumentedClass("byCRef"),
        [&promise](
            const InstrumentedClass & byValue, const InstrumentedClass & byRef, const InstrumentedClass & byCRef) {
            promise.set_value(make_tuple(byValue.id(), byRef.id(), byCRef.id()));
        });

    auto [idValue, idRef, idCRef] = promise.get_future().get();

    EXPECT_EQ(idValue, "M(M(M(byValue)))");
    EXPECT_EQ(idRef, "M(M(byRef))");
    EXPECT_EQ(idCRef, "M(M(byCRef))");
}

TEST(FreeFunction, ExecuteOnTaskQueueUsingLambda2Levels) {
    TaskQueue taskQueue;
    promise<tuple<string, string, string>> promise;

    taskQueue.enqueue([&promise, &taskQueue, byValue = InstrumentedClass("byValue"), byRef = InstrumentedClass("byRef"),
                       byCRef = InstrumentedClass("byCRef")]() {
        taskQueue.enqueue(
            [&promise, byValue = move(byValue), byRef = move(byRef), byCRef = move(byCRef)]() mutable {
                freeAsyncFn(
                    byValue, byRef, byCRef,
                    [&promise](
                        const InstrumentedClass & byValue, const InstrumentedClass & byRef,
                        const InstrumentedClass & byCRef) {
                        promise.set_value(make_tuple(byValue.id(), byRef.id(), byCRef.id()));
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