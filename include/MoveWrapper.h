#pragma once

#include <type_traits>

namespace my_library {
    using namespace std;

    template <typename T>
    struct MoveWrapper {
        explicit MoveWrapper(T && value) noexcept : value_{move(value)} { }

        // copy acts like move
        MoveWrapper(const MoveWrapper & src) noexcept : value_{move(src.value_)} { }
        MoveWrapper(MoveWrapper && src) noexcept : value_{move(src.value_)} { }
        MoveWrapper & operator=(const MoveWrapper &) = delete;
        MoveWrapper & operator=(MoveWrapper &&) noexcept = delete;
        ~MoveWrapper() = default;

        T & value() & noexcept { return value_; }
        const T & value() const & noexcept { return value_; }
        T && value() && noexcept { return move(value_); }

    private:
        mutable T value_{};
    };

} // namespace my_library