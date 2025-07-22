#ifndef NPY_ARRAY_NPY_DYNAMIC_ARRAY_H_
#define NPY_ARRAY_NPY_DYNAMIC_ARRAY_H_

#include <string>
#include <string_view>

#include "absl/status/statusor.h"
#include "npy_array/dynamic_array.h"
#include "npy_array/npy_array.h"

namespace npy_array {

// Reads npy data from a string and decodes into a DynamicArray that keeps a
// copy of the data.
// Array shape is inferred from the npy header as is, but will be reversed if
// the npy array is not in fortran order.
absl::StatusOr<DynamicArray> DecodeDynamicArrayFromNpy(
    std::string_view npy_data);

// Reads npy data from a string and decodes into a DynamicArrayRef view of the
// data.
// Array shape is inferred from the npy header as is, but will be reversed if
// the npy array is not in fortran order.
absl::StatusOr<DynamicArrayRef> MakeDynamicArrayRefOfNpy(
    std::string& npy_data ABSL_ATTRIBUTE_LIFETIME_BOUND);

}  // namespace npy_array

#endif  // NPY_ARRAY_NPY_DYNAMIC_ARRAY_H_