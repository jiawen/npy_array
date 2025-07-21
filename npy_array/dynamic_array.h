#ifndef NPY_ARRAY_DYNAMIC_ARRAY_H_
#define NPY_ARRAY_DYNAMIC_ARRAY_H_

#include <cstdint>
#include <vector>

#include "absl/types/span.h"
#include "array/array.h"
#include "npy_array/compile_time_loop.h"
#include "npy_array/data_type.h"
#include "npy_array/half.h"

namespace npy_array {

class DynamicShape {
 public:
  // DynamicShape intentionally lacks a default constructor because it would be
  // the same as a scalar, which is uncommnon and surprising. It also makes the
  // expression `DynamicShape({})` ambiguous (it should explicitly create a
  // scalar).

  // Constructs a DynamicShape with the given extents, automatically deducing:
  // - mins to be all 0.
  // - strides to be the product of all previous extents, starting with 1
  // (i.e., a planar layout).
  explicit DynamicShape(absl::Span<const int64_t> extents);

  // Constructs a DynamicShape with the given mins, extents, and strides.
  // TODO(jiawen): take in a Span<nda::dim<>>?
  // TODO(jiawen): Test for unresolved strides?
  DynamicShape(absl::Span<const int64_t> mins,
               absl::Span<const int64_t> extents,
               absl::Span<const int64_t> strides);
  DynamicShape(const DynamicShape&) = default;
  DynamicShape(DynamicShape&&) = default;
  DynamicShape& operator=(const DynamicShape&) = default;
  DynamicShape& operator=(DynamicShape&&) = default;

  // Returns true if the shape has no elements. Note that a rank-zero shape is
  // not empty because it has one element.
  bool empty() const { return size() == 0; }

  // Synonym for NumElements() to support STL / gMock.
  int64_t size() const { return NumElements(); }

  // Returns a Span of all the dims in this shape.
  // TODO(jiawen): ABSL_LIFETIME_BOUND.
  absl::Span<nda::dim<>> dims() { return absl::MakeSpan(dims_); }
  absl::Span<const nda::dim<>> dims() const { return dims_; }

  // TODO(jiawen): assert.
  nda::dim<>& dim(int64_t d) { return dims_[d]; }
  const nda::dim<>& dim(int64_t d) const { return dims_[d]; }

  // The number of dimensions.
  int64_t rank() const { return dims_.size(); }

  // Returns the number of elements (product of extents). Note that a rank-zero
  // array is a scalar and has 1 element.
  int64_t NumElements() const;

  // TODO(jiawen): assert.
  int64_t min(int64_t d) const { return dims_[d].min(); }  // NOLINT
  int64_t extent(int64_t d) const { return dims_[d].extent(); }
  int64_t stride(int64_t d) const { return dims_[d].stride(); }

  // Given an n-dimensional index `indices`, returns its "flattened" index in
  // linear memory.
  //
  // For a scalar, `indices` is {} and evaluates to 0.
  int64_t FlatIndex(absl::Span<const int64_t> indices) const;

  // Some synonyms for commonly used dimensions and extents.
  // ! Not bounds checked.
  int64_t width() const { return extent(0); }
  int64_t height() const { return extent(1); }
  int64_t channels() const { return extent(2); }
  int64_t depth() const { return extent(2); }

  int64_t rows() const { return extent(0); }
  int64_t cols() const { return extent(1); }
  int64_t slices() const { return extent(2); }

  // Some common names for dimensions (not bounds checked).
  // x, y, z, w -> 0, 1, 2, 3.
  // c -> 2.
  // i, j, k -> 0, 1, 2.
  nda::dim<>& x() { return dims_[0]; }
  const nda::dim<>& x() const { return dims_[0]; }
  nda::dim<>& y() { return dims_[1]; }
  const nda::dim<>& y() const { return dims_[1]; }
  nda::dim<>& z() { return dims_[2]; }
  const nda::dim<>& z() const { return dims_[2]; }
  nda::dim<>& c() { return dims_[2]; }
  const nda::dim<>& c() const { return dims_[2]; }
  nda::dim<>& w() { return dims_[3]; }
  const nda::dim<>& w() const { return dims_[3]; }

  nda::dim<>& i() { return dims_[0]; }
  const nda::dim<>& i() const { return dims_[0]; }
  nda::dim<>& j() { return dims_[1]; }
  const nda::dim<>& j() const { return dims_[1]; }
  nda::dim<>& k() { return dims_[2]; }
  const nda::dim<>& k() const { return dims_[2]; }

 private:
  std::vector<nda::dim<>> dims_;
};

// TODO(jiawen): `data` can be const? Make that a template.
// Will then need std::enable_if to disable some of the mutable methods.
// TODO(jiawen): Add DynamicArrayRef<[const] T>.
class DynamicArrayRef {
 public:
  DynamicArrayRef(uint8_t* data, DataType data_type, const DynamicShape& shape);
  DynamicArrayRef(const DynamicArrayRef&) = default;
  DynamicArrayRef& operator=(const DynamicArrayRef&) = default;
  DynamicArrayRef(DynamicArrayRef&&) = default;
  DynamicArrayRef& operator=(DynamicArrayRef&&) = default;

  // Returns true if the array has no elements. Note that a rank-zero array is
  // not empty because it has one element.
  bool empty() const { return shape_.empty(); }

  // The total number of bytes occupied by the data.
  // This is equal to size() * elementSize(data_type()).
  int64_t TotalSizeBytes() const;

  DataType data_type() const { return data_type_; }

  // TODO(jiawen): lifetime_bound
  const DynamicShape& shape() const { return shape_; }

  // The number of bytes occupied by each element.
  // Equal to elementSize(dataType()).
  int64_t ElementSizeBytes() const { return ElementSize(data_type_); }

  // The number of dimensions.
  int64_t rank() const { return shape_.rank(); }

  // The number of elements.
  int64_t NumElements() const { return shape_.NumElements(); }

  // Synonym for numElements() to support STL / gMock.
  int64_t size() const { return NumElements(); }

  // Some synonyms for commonly used dimensions and extents.
  // ! Not bounds checked.
  int64_t width() const { return shape_.width(); }
  int64_t height() const { return shape_.height(); }
  int64_t channels() const { return shape_.channels(); }
  int64_t depth() const { return shape_.depth(); }

  int64_t rows() const { return shape_.rows(); }
  int64_t cols() const { return shape_.cols(); }
  int64_t slices() const { return shape_.slices(); }

  // Some common names for dimensions (not bounds checked).
  // x, y, z, w -> 0, 1, 2, 3.
  // c -> 2.
  // i, j, k -> 0, 1, 2.
  nda::dim<>& x() { return shape_.x(); }
  const nda::dim<>& x() const { return shape_.x(); }
  nda::dim<>& y() { return shape_.y(); }
  const nda::dim<>& y() const { return shape_.y(); }
  nda::dim<>& z() { return shape_.z(); }
  const nda::dim<>& z() const { return shape_.z(); }
  nda::dim<>& c() { return shape_.c(); }
  const nda::dim<>& c() const { return shape_.c(); }
  nda::dim<>& w() { return shape_.w(); }
  const nda::dim<>& w() const { return shape_.w(); }

  nda::dim<>& i() { return shape_.i(); }
  const nda::dim<>& i() const { return shape_.i(); }
  nda::dim<>& j() { return shape_.j(); }
  const nda::dim<>& j() const { return shape_.j(); }
  nda::dim<>& k() { return shape_.k(); }
  const nda::dim<>& k() const { return shape_.k(); }

  // Retrieves the element at the given indices.
  template <typename T>
  T At(absl::Span<const int64_t> indices) const;

  // Retrieves a mutable reference to the element at the given indices.
  template <typename T>
  T& At(absl::Span<const int64_t> indices);

  // Same as at<T>(indices) = value, but deduces `T` from `value` so the caller
  // can write `.set(indices, value)`.
  template <typename T>
  void Set(absl::Span<const int64_t> indices, T value) {
    At<T>(indices) = value;
  }

  // Returns a pointer to the element at the given indices.
  template <typename T>
  const T* ElementPtr(absl::Span<const int64_t> indices) const;

  // Returns a mutable pointer to the element at the given indices.
  template <typename T>
  T* ElementPtr(absl::Span<const int64_t> indices);

  // Returns a pointer to the first element.
  const uint8_t* data() const { return data_; }
  uint8_t* data() { return data_; }

  // Returns a pointer to the first element as a T*.
  template <typename T>
  const T* data() const {
    return reinterpret_cast<const T*>(data());
  }

  template <typename T>
  T* data() {
    return reinterpret_cast<T*>(data());
  }

 private:
  uint8_t* data_;
  DataType data_type_;
  DynamicShape shape_;
};

// An n-dimensional // An n-dimensional array with dynamic data type and shape.
// TODO(jiawen,yyuting):
// - npy file support.
// - data() -> uint8_t*. Should we guarantee that it's nullptr? We don't need to
// and it simplifies the logic.
// - data<T>() -> T*.
class DynamicArray {
 public:
  DynamicArray(DataType data_type, absl::Span<const int64_t> extents);
  DynamicArray(const DynamicArray&) = default;
  DynamicArray& operator=(const DynamicArray&) = default;

  // TODO(jiawen): Test this - does it free memory? (Destructive move).
  DynamicArray(DynamicArray&&) = default;
  DynamicArray& operator=(DynamicArray&&) = default;

  // TODO(jiawen): Non-const for now.
  // TODO(jiawen): mark this class ABSl_OWNER. Does this still require
  // LIFETIME_BOUND?
  DynamicArrayRef ref();

  // Implicit conversion to a DynamicArrayRef.
  operator DynamicArrayRef() { return ref(); }

  // Returns true if the array has no elements. Note that a rank-zero array
  // is not empty because it has one element.
  bool empty() const { return shape_.empty(); }

  // The total number of bytes occupied by the data.
  // This is equal to size() * elementSize(data_type()).
  int64_t TotalSizeBytes() const { return NumElements() * ElementSizeBytes(); }

  DataType data_type() const { return data_type_; }

  const DynamicShape& shape() const { return shape_; }

  // The number of bytes occupied by each element.
  // Equal to elementSize(dataType()).
  int64_t ElementSizeBytes() const { return ElementSize(data_type_); }

  // The number of dimensions.
  int64_t rank() const { return shape_.rank(); }

  // The number of elements.
  int64_t NumElements() const { return shape_.NumElements(); }

  // Synonym for NumElements() to support STL / gMock.
  int64_t size() const { return NumElements(); }

  // Some synonyms for commonly used dimensions and extents.
  // ! Not bounds checked.
  int64_t width() const { return shape_.width(); }
  int64_t height() const { return shape_.height(); }
  int64_t channels() const { return shape_.channels(); }
  int64_t depth() const { return shape_.depth(); }

  int64_t rows() const { return shape_.rows(); }
  int64_t cols() const { return shape_.cols(); }
  int64_t slices() const { return shape_.slices(); }

  // Some common names for dimensions (not bounds checked).
  // x, y, z, w -> 0, 1, 2, 3.
  // c -> 2.
  // i, j, k -> 0, 1, 2.
  nda::dim<>& x() { return shape_.x(); }
  const nda::dim<>& x() const { return shape_.x(); }
  nda::dim<>& y() { return shape_.y(); }
  const nda::dim<>& y() const { return shape_.y(); }
  nda::dim<>& z() { return shape_.z(); }
  const nda::dim<>& z() const { return shape_.z(); }
  nda::dim<>& c() { return shape_.c(); }
  const nda::dim<>& c() const { return shape_.c(); }
  nda::dim<>& w() { return shape_.w(); }
  const nda::dim<>& w() const { return shape_.w(); }

  nda::dim<>& i() { return shape_.i(); }
  const nda::dim<>& i() const { return shape_.i(); }
  nda::dim<>& j() { return shape_.j(); }
  const nda::dim<>& j() const { return shape_.j(); }
  nda::dim<>& k() { return shape_.k(); }
  const nda::dim<>& k() const { return shape_.k(); }

  // Retrieves the element at the given indices.
  template <typename T>
  T At(absl::Span<const int64_t> indices) const {
    // TODO(jiawen): Get rid of this const_cast by implementing cref().
    return const_cast<DynamicArray&>(*this).ref().At<T>(indices);
  }

  // Retrieves a mutable reference to the element at the given indices.
  template <typename T>
  T& At(absl::Span<const int64_t> indices) {
    return ref().At<T>(indices);
  }

  // Same as at<T>(indices) = value, but deduces `T` from `value` so the
  // caller can write `.set(indices, value)`.
  template <typename T>
  void Set(absl::Span<const int64_t> indices, T value) {
    At<T>(indices) = value;
  }

  // Returns a pointer to the element at the given indices.
  template <typename T>
  const T* ElementPtr(absl::Span<const int64_t> indices) const;

  // Returns a mutable pointer to the element at the given indices.
  template <typename T>
  T* ElementPtr(absl::Span<const int64_t> indices);

  // Returns a pointer to the first element.
  const uint8_t* data() const { return data_.data(); }
  uint8_t* data() { return data_.data(); }

  // Returns a pointer to the first element as a T*.
  template <typename T>
  const T* data() const {
    return reinterpret_cast<const T*>(data());
  }

  template <typename T>
  T* data() {
    return reinterpret_cast<T*>(data());
  }

 private:
  DataType data_type_;
  DynamicShape shape_;
  std::vector<uint8_t> data_;
};

// Converts a static shape into a dynamic shape.
template <size_t Rank>
DynamicShape MakeDynamicShape(const nda::shape_of_rank<Rank>& shape);

// TODO(jiawen): Lifetime bound.
// Note that `T` can be const qualified.
template <typename T, size_t Rank>
nda::array_ref_of_rank<T, Rank> ArrayRefOf(const DynamicArrayRef& dar);

// TODO(jiawen):
// - Assert T is not const.
// - Assert T is from a valid list and explicitly instantiate?
// - Explicitly instantiate over Rank?
// - Using nda::array_ref_of_rank<T, Rank> is annoying - they cannot be deduced.
// But that's probably fine. Using a generic ArrayRefType is brittle too, until
// we use concepts.
template <typename T, size_t Rank>
DynamicArrayRef DynamicArrayRefOf(const nda::array_ref_of_rank<T, Rank>& ar);

// ----- Implementation of template functions -----
template <typename T, size_t Rank>
nda::array_ref_of_rank<T, Rank> ArrayRefOf(const DynamicArrayRef& dar) {
  if (dar.data_type() != DataTypeFor<std::remove_const_t<T>>()) {
    return {};
  }
  if (dar.rank() != Rank) {
    return {};
  }

  nda::shape_of_rank<Rank> shape;
  ForRange<0, Rank>(
      [&]<size_t D>() { shape.template dim<D>() = dar.shape().dims()[D]; });

  return nda::make_array_ref(dar.data<T>(), shape);
}

template <typename T, size_t Rank>
DynamicArrayRef DynamicArrayRefOf(const nda::array_ref_of_rank<T, Rank>& ar) {
  nda::shape_of_rank<Rank> ar_shape = ar.shape();
  DynamicShape dynamic_shape = MakeDynamicShape<Rank>(ar_shape);

  return DynamicArrayRef(reinterpret_cast<uint8_t*>(ar.data()),
                         DataTypeFor<T>(), dynamic_shape);
}

template <size_t Rank>
DynamicShape MakeDynamicShape(const nda::shape_of_rank<Rank>& shape) {
  std::array<int64_t, Rank> mins;
  std::array<int64_t, Rank> extents;
  std::array<int64_t, Rank> strides;
  // TODO(jiawen): Can use shape.extent(), which returns an index_type, which is
  // a tuple<index_t, ...>. Then use tuple_to_array to turn it into an array.
  ForRange<0, Rank>([&]<size_t D>() {
    mins[D] = shape.template dim<D>().min();
    extents[D] = shape.template dim<D>().extent();
    strides[D] = shape.template dim<D>().stride();
  });
  return DynamicShape(mins, extents, strides);
}

// ----- Explicit template declarations -----
extern template int8_t DynamicArrayRef::At<int8_t>(
    absl::Span<const int64_t>) const;
extern template int16_t DynamicArrayRef::At<int16_t>(
    absl::Span<const int64_t>) const;
extern template int32_t DynamicArrayRef::At<int32_t>(
    absl::Span<const int64_t>) const;
extern template int64_t DynamicArrayRef::At<int64_t>(
    absl::Span<const int64_t>) const;

extern template uint8_t DynamicArrayRef::At<uint8_t>(
    absl::Span<const int64_t>) const;
extern template uint16_t DynamicArrayRef::At<uint16_t>(
    absl::Span<const int64_t>) const;
extern template uint32_t DynamicArrayRef::At<uint32_t>(
    absl::Span<const int64_t>) const;
extern template uint64_t DynamicArrayRef::At<uint64_t>(
    absl::Span<const int64_t>) const;

extern template half DynamicArrayRef::At<half>(absl::Span<const int64_t>) const;
extern template float DynamicArrayRef::At<float>(
    absl::Span<const int64_t>) const;
extern template double DynamicArrayRef::At<double>(
    absl::Span<const int64_t>) const;

extern template int8_t& DynamicArrayRef::At<int8_t>(absl::Span<const int64_t>);
extern template int16_t& DynamicArrayRef::At<int16_t>(
    absl::Span<const int64_t>);
extern template int32_t& DynamicArrayRef::At<int32_t>(
    absl::Span<const int64_t>);
extern template int64_t& DynamicArrayRef::At<int64_t>(
    absl::Span<const int64_t>);

extern template uint8_t& DynamicArrayRef::At<uint8_t>(
    absl::Span<const int64_t>);
extern template uint16_t& DynamicArrayRef::At<uint16_t>(
    absl::Span<const int64_t>);
extern template uint32_t& DynamicArrayRef::At<uint32_t>(
    absl::Span<const int64_t>);
extern template uint64_t& DynamicArrayRef::At<uint64_t>(
    absl::Span<const int64_t>);

extern template half& DynamicArrayRef::At<half>(absl::Span<const int64_t>);
extern template float& DynamicArrayRef::At<float>(absl::Span<const int64_t>);
extern template double& DynamicArrayRef::At<double>(absl::Span<const int64_t>);

}  // namespace npy_array

#endif  // NPY_ARRAY_DYNAMIC_ARRAY_H_
