#include "npy_array/data_type.h"

namespace npy_array {

size_t ElementSize(DataType dt) {
  switch (dt) {
    case DataType::kUndefined:
      return 0;
    case DataType::kInt8:
      return sizeof(int8_t);
    case DataType::kInt16:
      return sizeof(int16_t);
    case DataType::kInt32:
      return sizeof(int32_t);
    case DataType::kInt64:
      return sizeof(int64_t);
    case DataType::kUint8:
      return sizeof(uint8_t);
    case DataType::kUint16:
      return sizeof(uint16_t);
    case DataType::kUint32:
      return sizeof(uint32_t);
    case DataType::kUint64:
      return sizeof(uint64_t);
    case DataType::kFloat16:
      return sizeof(half);
    case DataType::kFloat32:
      return sizeof(float);
    case DataType::kFloat64:
      return sizeof(double);
  }
}

template <typename T>
DataType DataTypeFor() {
  return DataType::kUndefined;
}

template <>
DataType DataTypeFor<int8_t>() {
  return DataType::kInt8;
}

template <>
DataType DataTypeFor<int16_t>() {
  return DataType::kInt16;
}

template <>
DataType DataTypeFor<int32_t>() {
  return DataType::kInt32;
}

template <>
DataType DataTypeFor<int64_t>() {
  return DataType::kInt64;
}

template <>
DataType DataTypeFor<uint8_t>() {
  return DataType::kUint8;
}

template <>
DataType DataTypeFor<uint16_t>() {
  return DataType::kUint16;
}

template <>
DataType DataTypeFor<uint32_t>() {
  return DataType::kUint32;
}

template <>
DataType DataTypeFor<uint64_t>() {
  return DataType::kUint64;
}

template <>
DataType DataTypeFor<half>() {
  return DataType::kFloat16;
}

template <>
DataType DataTypeFor<float>() {
  return DataType::kFloat32;
}

template <>
DataType DataTypeFor<double>() {
  return DataType::kFloat64;
}

}  // namespace npy_array
