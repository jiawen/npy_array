#ifndef NPY_ARRAY_GTEST_HALF_H_
#define NPY_ARRAY_GTEST_HALF_H_

#include <string>

#include "npy_array/half.h"

// Temporary solution for missing GetTypeName<half> symbol.
//
// TODO(jiawen): Resolve GetTypeName<half> missing symbol hack.
//
// As of Xcode 16, it complains with:
// Undefined symbols for architecture arm64:
//   "typeinfo for _Float16", referenced from:
//       std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> testing::internal::GetTypeName<_Float16>() in dynamic_array_test.o

namespace testing {
namespace internal {

template <>
std::string GetTypeName<npy_array::half>() {
  return "half";
}

}  // namespace internal
}  // namespace testing

#endif  // NPY_ARRAY_GTEST_HALF_H_
