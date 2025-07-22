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

std::vector<int64_t> GetNpyExtents(
    const npy_array::internal::NpyHeader& npy_header) {
  if (npy_header.fortran_order) {
    return std::vector<int64_t>(npy_header.shape.begin(),
                                npy_header.shape.end());
  } else {
    return std::vector<int64_t>(npy_header.shape.rbegin(),
                                npy_header.shape.rend());
  }
}

absl::Status VerifyTypeAndExtents(const DataType data_type,
                                  const std::vector<int64_t>& extents) {
  // Verify data type is valid.
  if (data_type == DataType::kUndefined) {
    return absl::InvalidArgumentError("Unknown data type.");
  }

  // Verify extents are not negative.
  for (const auto& extent : extents) {
    if (extent < 0) {
      return absl::InvalidArgumentError("Negative extent not supported.");
    }
  }
  return absl::OkStatus();
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

  const DataType data_type =
      GetDataType(npy_header.type_char, npy_header.word_size);
  const absl::Status status = VerifyTypeAndExtents(data_type, extents);
  if (!status.ok()) {
    return status;
  }

  DynamicArray arr(data_type, extents);

  memcpy(arr.data(), npy_data.begin() + npy_header.data_start_offset,
         expected_data_size);

  return arr;
}

absl::StatusOr<DynamicArrayRef> DecodeDynamicArrayRefFromNpy(
    std::string& npy_data) {
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

  const DataType data_type =
      GetDataType(npy_header.type_char, npy_header.word_size);
  const absl::Status status = VerifyTypeAndExtents(data_type, extents);
  if (!status.ok()) {
    return status;
  }

  return DynamicArrayRef(reinterpret_cast<uint8_t*>(
                             npy_data.data() + npy_header.data_start_offset),
                         data_type, DynamicShape(extents));
}

}  // namespace npy_array