#pragma once

#include <tuple>
#include <type_traits>

namespace my_library {
    namespace detail {
        template <bool is_reference, typename T>
        auto conditional_move(T & t) -> std::conditional_t<is_reference, T &, T &&> {
            if constexpr (is_reference) {
                return t;
            } else {
                return static_cast<T &&>(t);
            }
        }

        template <class T, class SrcTuple, std::size_t... I>
        auto make_from_tuple_impl(SrcTuple & src, std::index_sequence<I...>) {
            return std::forward_as_tuple(
                conditional_move<std::is_reference_v<std::tuple_element_t<I, T>>>(std::get<I>(src))...);
        }
    } // namespace detail

    template <typename... T1>
    struct TupleConvertor {
        explicit TupleConvertor(std::tuple<T1...> & src) : src_(src) {
        }

        template <typename DestTupleType>
        auto convert() {
            return detail::make_from_tuple_impl<DestTupleType>(
                src_, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<DestTupleType>>>{});
        }

    private:
        std::tuple<T1...> & src_;
    };
} // namespace my_library
