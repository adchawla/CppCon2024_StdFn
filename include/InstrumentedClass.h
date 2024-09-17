
#pragma once

#include <functional>
#include <string>

namespace my_library {

    struct InstrumentedClass {
        explicit InstrumentedClass(std::string id) : id_(std::move(id)) {
        }
        ~InstrumentedClass() = default;

        // id_ = "C(" + id_ + ")"
        InstrumentedClass(const InstrumentedClass & other);
        // id_ = "M(" + id_ + ")"
        InstrumentedClass(InstrumentedClass && other) noexcept;
        // id_ = "c=(" + id_ + ")"
        InstrumentedClass & operator=(const InstrumentedClass & other);
        // id_ = "m=(" + id_ + ")"
        InstrumentedClass & operator=(InstrumentedClass && other) noexcept;

        const std::string & id() const noexcept {
            return id_;
        }

    protected:
        std::string id_;
    };
} // namespace my_library
