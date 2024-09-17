#include "InstrumentedClass.h"

namespace {
    std::string generateNewId(const char * operation, const std::string & id) {
        return std::string(operation) + "(" + id + ")";
    }
} // namespace

namespace my_library {

    InstrumentedClass::InstrumentedClass(const InstrumentedClass & other) : id_(generateNewId("C", other.id_)) {
    }

    InstrumentedClass::InstrumentedClass(InstrumentedClass && other) noexcept : id_(generateNewId("M", other.id_)) {
    }

    InstrumentedClass & InstrumentedClass::operator=(const InstrumentedClass & other) {
        id_ = generateNewId("c=", other.id_);
        return *this;
    }

    InstrumentedClass & InstrumentedClass::operator=(InstrumentedClass && other) noexcept {
        id_ = generateNewId("m=", other.id_);
        return *this;
    }
} // namespace my_library