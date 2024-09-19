#pragma once
#include "TupleConvertor.h"
#include "function_traits.h"

#include <tuple>
#include <type_traits>

namespace my_library {
    namespace detail {
        template <class T>
        struct unwrap_refwrapper {
            using type = T;
        };

        template <class T>
        struct unwrap_refwrapper<std::reference_wrapper<T>> {
            using type = T &;
        };

        template <class T>
        using unwrap_decay_t = typename unwrap_refwrapper<std::decay_t<T>>::type;
        // or use unwrap_ref_decay_t (since C++20)

        template <class... Types>
        constexpr // since C++14
            std::tuple<unwrap_decay_t<Types>...>
            make_tuple(Types &&... args) {
            return std::tuple<unwrap_decay_t<Types>...>(std::forward<Types>(args)...);
        }
    } // namespace detail

    template <typename... Args>
    struct Holder {
        std::tuple<std::decay_t<Args>...> args;

        template <typename... TArgs>
        explicit Holder(TArgs &&... args) : args(std::forward<TArgs>(args)...) {
        }

        template <typename Callable>
        void invoke(Callable && fn) {
            std::apply(fn, std::move(args));
        }

        template <typename Callable>
        void invokeEx(Callable && fn) {
            using DestTupleType = function_traits_args_tuple_t<std::decay_t<Callable>>;
            auto destT = TupleConvertor(args);
            std::apply(fn, destT.template convert<DestTupleType>());
        }
    };

    template <typename... Args>
    std::unique_ptr<Holder<Args...>> make_unique_holder(Args &&... args) {
        return std::make_unique<Holder<Args...>>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    auto make_unique_tuple(Args &&... args) {
        using TupleType = std::tuple<std::decay_t<Args>...>;
        return std::make_unique<TupleType>(std::forward<Args>(args)...);
    }

} // namespace my_library
