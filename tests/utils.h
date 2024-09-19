#pragma once

#include "ConditionalStream.h"
#include "InstrumentedClass.h"

#include <functional>
#include <vector>

namespace my_test::utils {
    using namespace my_library;
    using CallbackFn = std::function<void(std::vector<std::string>)>;

    inline void asyncFn(
        InstrumentedClass byValue, InstrumentedClass & byRef, const InstrumentedClass & byCRef,
        const std::function<void(std::vector<std::string>)> & callbackFn) {
        callbackFn(std::vector{byValue.id(), byRef.id(), byCRef.id()});
    }

    inline void asyncFn2(
        InstrumentedClass byValue, const InstrumentedClass & byCRef,
        const std::function<void(std::vector<std::string>)> & callbackFn) {
        callbackFn(std::vector{byValue.id(), byCRef.id()});
    }

    inline void fn(InstrumentedClass byValue, InstrumentedClass & byRef, const InstrumentedClass & byCRef) {
        OSTREAM << byValue.id() << ", " << byRef.id() << ", " << byCRef.id() << std::endl;
    }

    inline void fn2(InstrumentedClass byValue, const InstrumentedClass & byCRef) {
        OSTREAM << byValue.id() << ", " << byCRef.id() << std::endl;
    }
} // namespace my_test::utils