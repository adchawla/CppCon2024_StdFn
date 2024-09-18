#include "ConditionalStream.h"
#include "Holder.h"
#include "InstrumentedClass.h"
#include "MoveWrapper.h"
#include "TaskQueue.h"
#include "utils.h"

#include <future>
#include <gtest/gtest.h>

using namespace my_library;
using namespace std;
using namespace my_test::utils;

TEST(PackingData, GoldStandard) {
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

    std::async(launch::async, asyncFn, move(byValue), move(byRef), move(byCRef), move(cbLambda)).wait();
}

TEST(PackingData, ByHand) {
    TaskQueue taskQueue;

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

    struct Holder {
        InstrumentedClass byValue;
        InstrumentedClass byRef;
        InstrumentedClass byCRef;
        CallbackFn callbackFn;

        Holder(
            InstrumentedClass && byValue, InstrumentedClass && byRef, InstrumentedClass && byCRef,
            CallbackFn && callbackFn)
            : byValue(move(byValue)), byRef(move(byRef)), byCRef(move(byCRef)), callbackFn(std::move(callbackFn)) {
        }
    };

    auto holder = make_shared<Holder>(move(byValue), move(byRef), move(byCRef), move(cbLambda));

    taskQueue.enqueue([holder = move(holder)] {
        asyncFn(move(holder->byValue), holder->byRef, holder->byCRef, move(holder->callbackFn));
    });
    taskQueue.waitForAllPreviousTasks();
}

TEST(PackingData, ByHandAndMoveWrapper) {
    TaskQueue taskQueue;

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

    struct Holder {
        InstrumentedClass byValue;
        InstrumentedClass byRef;
        InstrumentedClass byCRef;
        CallbackFn callbackFn;

        Holder(
            InstrumentedClass && byValue, InstrumentedClass && byRef, InstrumentedClass && byCRef,
            CallbackFn && callbackFn)
            : byValue(move(byValue)), byRef(move(byRef)), byCRef(move(byCRef)), callbackFn(std::move(callbackFn)) {
        }
    };

    auto holder = MoveWrapper(make_unique<Holder>(move(byValue), move(byRef), move(byCRef), move(cbLambda)));

    taskQueue.enqueue([holder] {
        auto & holderRef = holder.value();
        asyncFn(move(holderRef->byValue), holderRef->byRef, holderRef->byCRef, move(holderRef->callbackFn));
    });
    taskQueue.waitForAllPreviousTasks();
}

TEST(PackingData, ByTuple2) {
    TaskQueue taskQueue;

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

    auto t = make_unique_tuple(std::move(byValue), std::move(byCRef), std::move(cbLambda));

    taskQueue.enqueue([mt = MoveWrapper(std::move(t))] {
        apply(asyncFn2, std::move(*mt.value()));
    });
    taskQueue.waitForAllPreviousTasks();
}

TEST(PackingData, ByHolder) {
    TaskQueue taskQueue;

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

    auto uPtr = make_unique_holder(move(byValue), move(byRef), move(byCRef), move(cbLambda));

    using HaveType = std::tuple<InstrumentedClass, InstrumentedClass, const InstrumentedClass, CallbackFn>;
    using WantType = std::tuple<InstrumentedClass, InstrumentedClass &, const InstrumentedClass &, CallbackFn>;
    taskQueue.enqueue([holder = MoveWrapper(std::move(uPtr))] {
        auto & tuple = holder.value()->args;
        std::apply(
            asyncFn,
            WantType{
                std::move(std::get<0>(tuple)), std::get<1>(tuple), std::get<2>(tuple), std::move(std::get<3>(tuple))});
    });
    taskQueue.waitForAllPreviousTasks();
}

TEST(PackingData, ByHolder2) {
    TaskQueue taskQueue;

    InstrumentedClass byValue("byValue");
    InstrumentedClass byCRef("byCRef");
    InstrumentedClass capturedInCb("capturedInCb");

    auto cbLambda = [capturedInCb = move(capturedInCb)](const auto & ids) {
        for (const auto & id : ids) {
            OSTREAM << id << ", ";
        }
        OSTREAM << capturedInCb.id() << endl;
    };

    auto uPtr = make_unique_holder(move(byValue), move(byCRef), move(cbLambda));

    taskQueue.enqueue([holder = MoveWrapper(std::move(uPtr))] {
        holder.value()->invoke(asyncFn2);
    });
    taskQueue.waitForAllPreviousTasks();
}
