#include "ConditionalStream.h"
#include "Holder.h"
#include "InstrumentedClass.h"
#include "MoveWrapper.h"
#include "TaskQueue.h"
#include "TupleConvertor.h"
#include "utils.h"

#include <future>
#include <gtest/gtest.h>

using namespace my_library;
using namespace my_test::utils;

TEST(PackingData, GoldStandard) {
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
    std::async(
        std::launch::async, asyncFn, std::move(byValue), std::move(byRef), std::move(byCRef), std::move(cbLambda))
        .wait();
#else
    std::async(launch::async, asyncFn2, std::move(byValue), std::move(byCRef), std::move(cbLambda)).wait();
#endif
}

TEST(PackingData, ByHand) {
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

    struct Holder {
        InstrumentedClass byValue;
        InstrumentedClass byRef;
        InstrumentedClass byCRef;
        CallbackFn callbackFn;

        Holder(
            InstrumentedClass && byValue, InstrumentedClass && byRef, InstrumentedClass && byCRef,
            CallbackFn && callbackFn)
            : byValue(std::move(byValue))
            , byRef(std::move(byRef))
            , byCRef(std::move(byCRef))
            , callbackFn(std::move(callbackFn)) {
        }
    };

    auto holder =
        std::make_shared<Holder>(std::move(byValue), std::move(byRef), std::move(byCRef), std::move(cbLambda));

    taskQueue.enqueue([holder = std::move(holder)] {
        asyncFn(std::move(holder->byValue), holder->byRef, holder->byCRef, std::move(holder->callbackFn));
    });
    taskQueue.waitForAllPreviousTasks();
}

TEST(PackingData, ByHandAndMoveWrapper) {
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

    struct Holder {
        InstrumentedClass byValue;
        InstrumentedClass byRef;
        InstrumentedClass byCRef;
        CallbackFn callbackFn;

        Holder(
            InstrumentedClass && byValue, InstrumentedClass && byRef, InstrumentedClass && byCRef,
            CallbackFn && callbackFn)
            : byValue(std::move(byValue))
            , byRef(std::move(byRef))
            , byCRef(std::move(byCRef))
            , callbackFn(std::move(callbackFn)) {
        }
    };

    auto holder = MoveWrapper(
        std::make_unique<Holder>(std::move(byValue), std::move(byRef), std::move(byCRef), std::move(cbLambda)));

    taskQueue.enqueue([holder] {
        auto & holderRef = holder.value();
        asyncFn(std::move(holderRef->byValue), holderRef->byRef, holderRef->byCRef, std::move(holderRef->callbackFn));
    });
    taskQueue.waitForAllPreviousTasks();
}

TEST(PackingData, ByTuple2) {
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

    auto t = make_unique_tuple(std::move(byValue), std::move(byCRef), std::move(cbLambda));

    taskQueue.enqueue([mt = MoveWrapper(std::move(t))] {
        apply(asyncFn2, std::move(*mt.value()));
    });
    taskQueue.waitForAllPreviousTasks();
}

TEST(PackingData, ByHolderManualConversion) {
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

    auto uPtr = make_unique_holder(std::move(byValue), std::move(byRef), std::move(byCRef), std::move(cbLambda));

    // using HaveType = std::tuple<InstrumentedClass, InstrumentedClass, const InstrumentedClass, CallbackFn>;
    using WantType = std::tuple<InstrumentedClass &&, InstrumentedClass &, const InstrumentedClass &, CallbackFn>;
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

    auto cbLambda = [capturedInCb = std::move(capturedInCb)](const auto & ids) {
        for (const auto & id : ids) {
            OSTREAM << id << ", ";
        }
        OSTREAM << capturedInCb.id() << std::endl;
    };

    auto uPtr = make_unique_holder(std::move(byValue), std::move(byCRef), std::move(cbLambda));

    taskQueue.enqueue([holder = MoveWrapper(std::move(uPtr))] {
        holder.value()->invoke(asyncFn2);
    });
    taskQueue.waitForAllPreviousTasks();
}

TEST(PackingData, ByHolderAndTupleConversion) {
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

    auto uPtr = make_unique_holder(std::move(byValue), std::move(byRef), std::move(byCRef), std::move(cbLambda));

    // using HaveType = std::tuple<InstrumentedClass, InstrumentedClass, const InstrumentedClass, CallbackFn>;
    using WantType = std::tuple<InstrumentedClass, InstrumentedClass &, const InstrumentedClass &, CallbackFn>;
    taskQueue.enqueue([holder = MoveWrapper(std::move(uPtr))] {
        auto & tuple = holder.value()->args;
        std::apply(asyncFn, TupleConvertor(tuple).convert<WantType>());
    });
    taskQueue.waitForAllPreviousTasks();
}

TEST(PackingData, ByHolderFunctionTraitsAndTupleConversion) {
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

    auto uPtr =
        make_unique_holder(std::move(byValue), std::move(byRef), std::move(byCRef), CallbackFn{std::move(cbLambda)});

    using WantType = function_traits_args_tuple_t<decltype(asyncFn)>;
    taskQueue.enqueue([holder = MoveWrapper(std::move(uPtr))] {
        auto & tuple = holder.value()->args;
        std::apply(asyncFn, TupleConvertor(tuple).convert<WantType>());
    });
    taskQueue.waitForAllPreviousTasks();
}

TEST(PackingData, ByHolderInvokeEx) {
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

    auto uPtr =
        make_unique_holder(std::move(byValue), std::move(byRef), std::move(byCRef), CallbackFn(std::move(cbLambda)));

    taskQueue.enqueue([holder = MoveWrapper(std::move(uPtr))] {
        holder.value()->invokeEx(asyncFn);
    });
    taskQueue.waitForAllPreviousTasks();
}
