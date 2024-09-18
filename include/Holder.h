#pragma once
#include <tuple>
#include <type_traits>

namespace my_library {
    using namespace std;
    namespace detail {
        template <class T>
        struct unwrap_refwrapper {
            using type = T;
        };

        template <class T>
        struct unwrap_refwrapper<reference_wrapper<T>> {
            using type = T &;
        };

        template <class T>
        using unwrap_decay_t = typename unwrap_refwrapper<typename decay<T>::type>::type;
        // or use unwrap_ref_decay_t (since C++20)

        template <class... Types>
        constexpr // since C++14
            tuple<unwrap_decay_t<Types>...>
            make_tuple(Types &&... args) {
            return tuple<unwrap_decay_t<Types>...>(forward<Types>(args)...);
        }
    }

    template <typename... Args>
    struct Holder {
        tuple<decay_t<Args>...> args;

        template <typename... TArgs>
        explicit Holder(TArgs &&... args)
            : args(forward<TArgs>(args)...) {
        }

        template <typename Callable>
        void invoke(Callable && fn) {
            apply(fn, move(args));
        }
    };

    template<typename... Args>
    unique_ptr<Holder<Args...>> make_unique_holder(Args &&... args) {
        return make_unique<Holder<Args...>>(forward<Args>(args)...);
    }

}