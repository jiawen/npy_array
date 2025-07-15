#ifndef NPY_ARRAY_COMPILE_TIME_LOOP_H_
#define NPY_ARRAY_COMPILE_TIME_LOOP_H_

#include <utility>

// A collection of utilities for iterating over types and values at compile
// time.
//
// Borrowed liberally from:
// https://vittorioromeo.com/index/blog/cpp20_lambdas_compiletime_for.html.

namespace npy_array {

// Compile-time iteration over a type list: applies the template function F<T>
// for each element T in `Ts`. E.g.,:
//
// forTypes<int, float, double>([&]<typename T>() {
//   // Do something with T.
//   if constexpr (std::is_same_as_v<T, int>) {
//     // Do something special for int.
//   } else {
//     // Handle floating point types in the default case.
//   }
// });
template <typename... Ts, typename F>
constexpr void forTypes(F&& f);

// Compile-time iteration over values: applies the template function F<X> for
// each element X in `Xs`. E.g.,:
//
// forValues<1, 2, 3, 4, 5>([&]<int Rank>() {
//   createPybind<ArrayOfRank<float, Rank>>>();
// });
template <auto... Xs, typename F>
constexpr void forValues(F&& f);

// Compile-time iteration over a range of values: applies the template function
// F<X> for each element X in the half-open interval [B, E). Note that negative
// numbers need to be parenthesized. E.g.,:
//
// forRange<(-10), 6>([&]<int X>() {
//   // Do something with the compile-time constant X in [-10, 6).
// });
template <auto B, auto E, typename F>
constexpr void forRange(F&& f);

// Same as forTypes, but also provides the index of the type in the list. E.g.,
// enumerateTypes<int, float>([&]<std::size_t I, typename T>() {
//   // Do something with I, T.
//   if constexpr (std::is_same_as_v<T, int>) {
//     static_assert(std::is_same_v<T, int>);
//   } else if constexpr (I == 1) {
//     static_assert(std::is_same_v<T, float>);
//   }
// });
template <typename... Ts, typename F>
constexpr void enumerateTypes(F&& f);

////////////////////////////////////////////////////////////////////////////////
// Implementation of template functions
////////////////////////////////////////////////////////////////////////////////

template <typename... Ts, typename F>
constexpr void forTypes(F&& f) {
  (f.template operator()<Ts>(), ...);
}

template <auto... Xs, typename F>
constexpr void forValues(F&& f) {
  (f.template operator()<Xs>(), ...);
}

template <auto B, auto E, typename F>
constexpr void forRange(F&& f) {
  using T = std::common_type_t<decltype(B), decltype(E)>;

  [&f]<auto... Xs>(std::integer_sequence<T, Xs...>) {
    forValues<(B + Xs)...>(f);
  }(std::make_integer_sequence<T, E - B>{});
}

template <typename... Ts, typename F>
constexpr void enumerateTypes(F&& f) {
  [&f]<auto... Is>(std::index_sequence<Is...>) {
    (f.template operator()<Is, Ts>(), ...);
  }(std::index_sequence_for<Ts...>{});
}

}  // namespace npy_array

#endif  // NPY_ARRAY_COMPILE_TIME_LOOP_H_
