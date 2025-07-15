#ifndef NPY_ARRAY_DATA_TYPE_H_
#define NPY_ARRAY_DATA_TYPE_H_

#include <cstdint>

#include "npy_array/half.h"

namespace npy_array {

// Underlying data type used by pixel formats.
enum class DataType {
  kUndefined,

  kInt8,
  kInt16,
  kInt32,
  kInt64,

  kUint8,
  kUint16,
  kUint32,
  kUint64,

  kFloat16,
  kFloat32,
  kFloat64,
};

template <typename Sink>
void AbslStringify(Sink& sink, DataType dt) {
  switch (dt) {
    case DataType::kUndefined:
      sink.Append("kUndefined");
      break;
    case DataType::kInt8:
      sink.Append("kInt8");
      break;
    case DataType::kInt16:
      sink.Append("kInt16");
      break;
    case DataType::kInt32:
      sink.Append("kInt32");
      break;
    case DataType::kInt64:
      sink.Append("kInt64");
      break;

    case DataType::kUint8:
      sink.Append("kUint8");
      break;
    case DataType::kUint16:
      sink.Append("kUint16");
      break;
    case DataType::kUint32:
      sink.Append("kUint32");
      break;
    case DataType::kUint64:
      sink.Append("kUint64");
      break;
    case DataType::kFloat16:
      sink.Append("kFloat16");
      break;
    case DataType::kFloat32:
      sink.Append("kFloat32");
      break;
    case DataType::kFloat64:
      sink.Append("kFloat64");
      break;
  }
}

// Returns the size of 1 element of type `dt`, in bytes.
size_t elementSize(DataType dt);

// Maps T to a DataType.
template <typename T>
DataType dataTypeFor();

}  // namespace npy_array

#endif  // NPY_ARRAY_DATA_TYPE_H_
