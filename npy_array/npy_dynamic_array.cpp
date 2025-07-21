#include "npy_array/npy_dynamic_array.h"

namespace npy_array {

namespace {

// Returns corresponding DataType based on type_char and word_size.
DataType GetDataType(char type_char, size_t word_size) {
  switch (type_char) {
    case 'i': {
      if (word_size == sizeof(int8_t)) {
        return DataType::kInt8;
      } else if (word_size == sizeof(int16_t)) {
        return DataType::kInt16;
      } else if (word_size == sizeof(int32_t)) {
        return DataType::kInt32;
      } else if (word_size == sizeof(int64_t)) {
        return DataType::kInt64;
      } else {
        return DataType::kUndefined;
      }
    }
    case 'u': {
      if (word_size == sizeof(uint8_t)) {
        return DataType::kUint8;
      } else if (word_size == sizeof(uint16_t)) {
        return DataType::kUint16;
      } else if (word_size == sizeof(uint32_t)) {
        return DataType::kUint32;
      } else if (word_size == sizeof(uint64_t)) {
        return DataType::kUint64;
      } else {
        return DataType::kUndefined;
      }
    }
    case 'f': {
      if (word_size == sizeof(half)) {
        return DataType::kFloat16;
      } else if (word_size == sizeof(float)) {
        return DataType::kFloat32;
      } else if (word_size == sizeof(double)) {
        return DataType::kFloat64;
      } else {
        return DataType::kUndefined;
      }
    }
    default:
      return DataType::kUndefined;
  }
}

const std::vector<int64_t> GetNpyExtents(
    npy_array::internal::NpyHeader npy_header) {
  if (!npy_header.fortran_order) {
    std::reverse(npy_header.shape.begin(), npy_header.shape.end());
  }

  return std::vector<int64_t>(npy_header.shape.begin(), npy_header.shape.end());
}

}  // namespace

absl::StatusOr<DynamicArray> DecodeDynamicArrayFromNpy(
    std::string_view npy_data) {
  auto npy_header = npy_array::internal::ReadHeader(npy_data);
  if (!npy_header.valid) {
    return absl::InvalidArgumentError("Invalid npy header");
  }

  const size_t expected_data_size =
      npy_header.total_element_count * npy_header.word_size;

  if (npy_header.data_start_offset + expected_data_size > npy_data.size()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Invalid npy data size: expected at least ", expected_data_size,
        " bytes, got ", (npy_data.size() - npy_header.data_start_offset),
        " bytes"));
  }

  const std::vector<int64_t> extents = GetNpyExtents(npy_header);

  DynamicArray arr(GetDataType(npy_header.type_char, npy_header.word_size),
                   extents);

  memcpy(arr.data(), npy_data.begin() + npy_header.data_start_offset,
         expected_data_size);

  return arr;
}

absl::StatusOr<DynamicArrayRef> DecodeDynamicArrayRefFromNpy(
    std::string& npy_data ABSL_ATTRIBUTE_LIFETIME_BOUND) {
  auto npy_header = npy_array::internal::ReadHeader(npy_data);
  if (!npy_header.valid) {
    return absl::InvalidArgumentError("Invalid npy header");
  }

  const size_t expected_data_size =
      npy_header.total_element_count * npy_header.word_size;

  if (npy_header.data_start_offset + expected_data_size > npy_data.size()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Invalid npy data size: expected at least ", expected_data_size,
        " bytes, got ", (npy_data.size() - npy_header.data_start_offset),
        " bytes"));
  }

  const std::vector<int64_t> extents = GetNpyExtents(npy_header);

  return DynamicArrayRef(
      reinterpret_cast<uint8_t*>(npy_data.data() +
                                 npy_header.data_start_offset),
      GetDataType(npy_header.type_char, npy_header.word_size),
      DynamicShape(extents));
}

}  // namespace npy_array