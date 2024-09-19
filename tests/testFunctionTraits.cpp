#include "function_traits.h"
#include "utils.h"

#include <gtest/gtest.h>

using namespace my_library;
using namespace my_test::utils;

namespace detail {
    template <class T>
    [[nodiscard]] inline constexpr int warnx(T v) {
        return 0;
    }
} // namespace detail

//
// To print the type of the parameter at compile time as part of error.
// usage: warn<Type>(t);
//
template <class T>
inline constexpr void warn() {
    detail::warnx<T *>({});
}

TEST(FunctionTraits, NormalFunctions) {
    auto id = function_traits_id_v<decltype(fn)>;
    EXPECT_EQ(id, "function_traits<R(Args...)>");
    EXPECT_EQ(function_traits_id_v<decltype(asyncFn)>, "function_traits<R(Args...)>");
    EXPECT_EQ(function_traits_arity_v<decltype(asyncFn)>, 4);

    using FnType = function_traits_args_tuple_t<decltype(asyncFn)>;
    using ExpectedType =
        std::tuple<InstrumentedClass, InstrumentedClass &, const InstrumentedClass &, const CallbackFn &>;
    static_assert(std::is_same_v<FnType, ExpectedType>);

    EXPECT_TRUE((std::is_same_v<FnType, ExpectedType>) );
}