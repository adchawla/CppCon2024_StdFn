#pragma once

#include <type_traits>

namespace my_library {
    template <typename T>
    struct MoveWrapper {
        explicit MoveWrapper(T && value) noexcept : value_{std::move(value)} {
        }

        // copy acts like move
        MoveWrapper(const MoveWrapper & src) noexcept : value_{std::move(src.value_)} {
        }
        MoveWrapper(MoveWrapper && src) noexcept : value_{std::move(src.value_)} {
        }
        MoveWrapper & operator=(const MoveWrapper &) = delete;
        MoveWrapper & operator=(MoveWrapper &&) noexcept = delete;
        ~MoveWrapper() = default;

        T & value() & noexcept {
            return value_;
        }
        const T & value() const & noexcept {
            return value_;
        }
        T && value() && noexcept {
            return std::move(value_);
        }

    private:
        mutable T value_{};
    };

} // namespace my_library