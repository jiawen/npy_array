#include "npy_array/dynamic_array.h"

#include <cassert>
#include <utility>

namespace npy_array {

DynamicShape::DynamicShape(absl::Span<const int64_t> extents)
    : dims_(extents.size()) {
  for (int64_t d = 0; d < rank(); ++d) {
    dims_[d].set_min(0);
    dims_[d].set_extent(extents[d]);
  }

  if (rank() > 0) {
    dims_[0].set_stride(1);
    for (int64_t d = 1; d < rank(); ++d) {
      dims_[d].set_stride(extents[d - 1] * dims_[d - 1].stride());
    }
  }
}

DynamicShape::DynamicShape(absl::Span<const int64_t> mins,
                           absl::Span<const int64_t> extents,
                           absl::Span<const int64_t> strides)
    : dims_(extents.size()) {
  assert(mins.size() == rank());
  assert(strides.size() == rank());

  for (int64_t d = 0; d < rank(); ++d) {
    dims_[d].set_min(mins[d]);
    dims_[d].set_extent(extents[d]);
    dims_[d].set_stride(strides[d]);
  }
}

int64_t DynamicShape::numElements() const {
  int64_t num_elements = 1;
  for (const auto& d : dims_) {
    num_elements *= d.extent();
  }
  return num_elements;
}

int64_t DynamicShape::flatIndex(absl::Span<const int64_t> indices) const {
  assert(indices.size() == rank());

  int64_t flat_index = 0;
  for (int64_t d = 0; d < rank(); ++d) {
    flat_index += (indices[d] - min(d)) * stride(d);  // NOLINT
  }
  return flat_index;
}

DynamicArrayRef::DynamicArrayRef(uint8_t* data, DataType data_type,
                                 const DynamicShape& shape)
    : data_(data), data_type_(data_type), shape_(shape) {}

int64_t DynamicArrayRef::totalSizeBytes() const {
  return numElements() * elementSizeBytes();
}

DynamicArray::DynamicArray(DataType data_type,
                           absl::Span<const int64_t> extents)
    : data_type_(data_type), shape_(extents), data_(totalSizeBytes()) {}

DynamicArrayRef DynamicArray::ref() {
  return DynamicArrayRef(data_.data(), data_type_, shape_);
}

template <typename T>
T DynamicArrayRef::at(absl::Span<const int64_t> indices) const {
  return *elementPtr<T>(indices);
}

template <typename T>
T& DynamicArrayRef::at(absl::Span<const int64_t> indices) {
  return *elementPtr<T>(indices);
}

template <typename T>
const T* DynamicArrayRef::elementPtr(absl::Span<const int64_t> indices) const {
  assert(dataTypeFor<T>() == data_type_);
  const int64_t element_index = shape_.flatIndex(indices);
  return data<T>() + element_index;
}

template <typename T>
T* DynamicArrayRef::elementPtr(absl::Span<const int64_t> indices) {
  assert(dataTypeFor<T>() == data_type_);
  const int64_t element_index = shape_.flatIndex(indices);
  return data<T>() + element_index;
}

template int8_t DynamicArrayRef::at<int8_t>(absl::Span<const int64_t>) const;
template int16_t DynamicArrayRef::at<int16_t>(absl::Span<const int64_t>) const;
template int32_t DynamicArrayRef::at<int32_t>(absl::Span<const int64_t>) const;
template int64_t DynamicArrayRef::at<int64_t>(absl::Span<const int64_t>) const;

template uint8_t DynamicArrayRef::at<uint8_t>(absl::Span<const int64_t>) const;
template uint16_t DynamicArrayRef::at<uint16_t>(
    absl::Span<const int64_t>) const;
template uint32_t DynamicArrayRef::at<uint32_t>(
    absl::Span<const int64_t>) const;
template uint64_t DynamicArrayRef::at<uint64_t>(
    absl::Span<const int64_t>) const;

template half DynamicArrayRef::at<half>(absl::Span<const int64_t>) const;
template float DynamicArrayRef::at<float>(absl::Span<const int64_t>) const;
template double DynamicArrayRef::at<double>(absl::Span<const int64_t>) const;

template int8_t& DynamicArrayRef::at<int8_t>(absl::Span<const int64_t>);
template int16_t& DynamicArrayRef::at<int16_t>(absl::Span<const int64_t>);
template int32_t& DynamicArrayRef::at<int32_t>(absl::Span<const int64_t>);
template int64_t& DynamicArrayRef::at<int64_t>(absl::Span<const int64_t>);

template uint8_t& DynamicArrayRef::at<uint8_t>(absl::Span<const int64_t>);
template uint16_t& DynamicArrayRef::at<uint16_t>(absl::Span<const int64_t>);
template uint32_t& DynamicArrayRef::at<uint32_t>(absl::Span<const int64_t>);
template uint64_t& DynamicArrayRef::at<uint64_t>(absl::Span<const int64_t>);

template half& DynamicArrayRef::at<half>(absl::Span<const int64_t>);
template float& DynamicArrayRef::at<float>(absl::Span<const int64_t>);
template double& DynamicArrayRef::at<double>(absl::Span<const int64_t>);

}  // namespace npy_array
