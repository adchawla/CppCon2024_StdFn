#pragma once

#include <functional>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace my_library {
    namespace details {
        template <typename R, typename... Args>
        struct function_traits_helper {
            using args_tuple_type = std::tuple<Args...>;
            using args_by_value_tuple_type = std::tuple<std::decay_t<Args>...>;

            using return_type = R;
            using return_by_value_type = std::decay_t<R>;

            static constexpr size_t arity = sizeof...(Args);
        };

    } // namespace details
    template <typename>
    struct function_traits final {};

#define CREATE_FN_SIGNATURE_TRAITS(...)                                                                                \
    template <typename R, typename... Args>                                                                            \
    struct function_traits<R(Args...) __VA_ARGS__> final : details::function_traits_helper<R, Args...> {               \
        static constexpr std::string_view kId = "function_traits<R(Args...)" #__VA_ARGS__ ">";                         \
    };

#define CREATE_FN_PTR_SIGNATURE_TRAITS(...)                                                                            \
    template <typename R, typename... Args>                                                                            \
    struct function_traits<R (*)(Args...) __VA_ARGS__> final : details::function_traits_helper<R, Args...> {           \
        static constexpr std::string_view kId = "function_traits<R(*)(Args...)" #__VA_ARGS__ ">";                      \
    };

    template <typename R, typename... Args>
    struct function_traits<std::function<R(Args...)>> final : details::function_traits_helper<R, Args...> {
        static constexpr std::string_view kId = "function_traits<std::function<R(Args...)>>";
    };

    template <typename... Args>
    using function_traits_args_tuple_t = typename function_traits<Args...>::args_tuple_type;
    template <typename... Args>
    using function_traits_args_by_value_tuple_t = typename function_traits<Args...>::args_by_value_tuple_type;
    template <typename... Args>
    using function_traits_return_t = typename function_traits<Args...>::return_type;
    template <typename... Args>
    using function_traits_return_by_value_t = typename function_traits<Args...>::return_by_value_type;
    template <typename... Args>
    constexpr size_t function_traits_arity_v = function_traits<Args...>::arity;
    template <typename... Args>
    constexpr std::string_view function_traits_id_v = function_traits<Args...>::kId;

    CREATE_FN_SIGNATURE_TRAITS();
    CREATE_FN_SIGNATURE_TRAITS(noexcept);
    CREATE_FN_PTR_SIGNATURE_TRAITS();
    CREATE_FN_PTR_SIGNATURE_TRAITS(noexcept);
#undef CREATE_FN_SIGNATURE_TRAITS
#undef CREATE_FN_PTR_SIGNATURE_TRAITS
#define AUTO_FORWARD(x) std::forward<decltype(x)>(x)
} // namespace my_library