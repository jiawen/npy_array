#include "npy_array/dynamic_array.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "npy_array/gtest_half.h"

using testing::IsEmpty;
using testing::Not;

namespace npy_array {
namespace {

template <typename T>
T Pattern(absl::Span<const int64_t> indices) {
  T result = T(42);
  for (int i = 0; i < indices.size(); ++i) {
    result += (i + 3) * indices[i];
  }
  return result;
}

// A different pattern from `pattern`/
template <typename T>
T Pattern2(absl::Span<const int64_t> indices) {
  // Really the same function, but with a different seed.
  T result = T(43);
  for (int i = 0; i < indices.size(); ++i) {
    result += (i + 3) * indices[i];
  }
  return result;
}

TEST(DynamicShapeTest, Scalar) {
  DynamicShape shape({});
  EXPECT_FALSE(shape.empty());
  EXPECT_EQ(shape.rank(), 0);
  EXPECT_EQ(shape.size(), 1);
  EXPECT_EQ(shape.FlatIndex({}), 0);
}

TEST(DynamicShapeTest, Empty) {
  {
    // clang-format nonsense.
    EXPECT_THAT(DynamicShape({0}), IsEmpty());
  }
  {
    EXPECT_THAT(DynamicShape({0, 0}), IsEmpty());
    EXPECT_THAT(DynamicShape({0, 1}), IsEmpty());
    EXPECT_THAT(DynamicShape({2, 0}), IsEmpty());
  }
  {
    EXPECT_THAT(DynamicShape({0, 0, 0}), IsEmpty());
    EXPECT_THAT(DynamicShape({0, 1, 2}), IsEmpty());
    EXPECT_THAT(DynamicShape({1, 0, 2}), IsEmpty());
    EXPECT_THAT(DynamicShape({1, 2, 0}), IsEmpty());
  }
}

TEST(DynamicShapeTest, RankOne) {
  const std::vector<int64_t> extents = {57};
  DynamicShape shape(extents);
  EXPECT_EQ(shape.rank(), 1);
  EXPECT_EQ(shape.NumElements(), 57);

  EXPECT_EQ(shape.min(0), 0);
  EXPECT_EQ(shape.extent(0), 57);
  EXPECT_EQ(shape.stride(0), 1);
}

TEST(DynamicShapeTest, RankTwo) {
  const std::vector<int64_t> extents = {57, 43};
  DynamicShape shape(extents);
  EXPECT_EQ(shape.rank(), 2);
  EXPECT_EQ(shape.NumElements(), 57 * 43);

  EXPECT_EQ(shape.min(0), 0);
  EXPECT_EQ(shape.extent(0), 57);
  EXPECT_EQ(shape.stride(0), 1);

  EXPECT_EQ(shape.min(1), 0);
  EXPECT_EQ(shape.extent(1), 43);
  EXPECT_EQ(shape.stride(1), 57);
}

TEST(DynamicShapeTest, RankThree) {
  const std::vector<int64_t> extents = {57, 43, 23};
  DynamicShape shape(extents);
  EXPECT_EQ(shape.rank(), 3);
  EXPECT_EQ(shape.NumElements(), 57 * 43 * 23);

  EXPECT_EQ(shape.min(0), 0);
  EXPECT_EQ(shape.extent(0), 57);
  EXPECT_EQ(shape.stride(0), 1);

  EXPECT_EQ(shape.min(1), 0);
  EXPECT_EQ(shape.extent(1), 43);
  EXPECT_EQ(shape.stride(1), 57);

  EXPECT_EQ(shape.min(2), 0);
  EXPECT_EQ(shape.extent(2), 23);
  EXPECT_EQ(shape.stride(2), 57 * 43);
}

TEST(DynamicShapeTest, MakeDynamicShape) {
  {
    nda::shape_of_rank<0> shape;

    DynamicShape dynamic_shape = MakeDynamicShape<0>(shape);
    EXPECT_EQ(dynamic_shape.rank(), 0);
    EXPECT_EQ(dynamic_shape.NumElements(), 1);
  }

  {
    nda::shape_of_rank<1> shape;
    shape.dim<0>().set_min(5);
    shape.dim<0>().set_extent(7);
    shape.dim<0>().set_stride(6);

    DynamicShape dynamic_shape = MakeDynamicShape<1>(shape);
    EXPECT_EQ(dynamic_shape.rank(), 1);
    EXPECT_EQ(dynamic_shape.NumElements(), 7);
    EXPECT_EQ(dynamic_shape.min(0), 5);
    EXPECT_EQ(dynamic_shape.extent(0), 7);
    EXPECT_EQ(dynamic_shape.stride(0), 6);
  }

  {
    nda::shape_of_rank<2> shape;
    shape.dim<0>().set_min(5);
    shape.dim<0>().set_extent(7);
    shape.dim<0>().set_stride(6);
    shape.dim<1>().set_min(3);
    shape.dim<1>().set_extent(5);
    shape.dim<1>().set_stride(7);

    DynamicShape dynamic_shape = MakeDynamicShape<2>(shape);
    EXPECT_EQ(dynamic_shape.rank(), 2);
    EXPECT_EQ(dynamic_shape.NumElements(), 35);
    EXPECT_EQ(dynamic_shape.min(0), 5);
    EXPECT_EQ(dynamic_shape.extent(0), 7);
    EXPECT_EQ(dynamic_shape.stride(0), 6);
    EXPECT_EQ(dynamic_shape.min(1), 3);
    EXPECT_EQ(dynamic_shape.extent(1), 5);
    EXPECT_EQ(dynamic_shape.stride(1), 7);
  }
}

template <typename T>
class DynamicArrayTest : public testing::Test {};

using MyTypes =
    testing::Types<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t,
                   uint32_t, uint64_t, half, float, double>;
TYPED_TEST_SUITE(DynamicArrayTest, MyTypes);

TYPED_TEST(DynamicArrayTest, Empty) {
  // Scalars are not empty.
  {
    EXPECT_THAT(DynamicArray(DataTypeFor<TypeParam>(), {}), Not(IsEmpty()));
  }

  {
    // clang-format nonsense.
    EXPECT_THAT(DynamicArray(DataTypeFor<TypeParam>(), {0}), IsEmpty());
  }
  {
    EXPECT_THAT(DynamicArray(DataTypeFor<TypeParam>(), {0, 0}), IsEmpty());
    EXPECT_THAT(DynamicArray(DataTypeFor<TypeParam>(), {0, 1}), IsEmpty());
    EXPECT_THAT(DynamicArray(DataTypeFor<TypeParam>(), {2, 0}), IsEmpty());
  }
  {
    EXPECT_THAT(DynamicArray(DataTypeFor<TypeParam>(), {0, 0, 0}), IsEmpty());
    EXPECT_THAT(DynamicArray(DataTypeFor<TypeParam>(), {0, 1, 2}), IsEmpty());
    EXPECT_THAT(DynamicArray(DataTypeFor<TypeParam>(), {1, 0, 2}), IsEmpty());
    EXPECT_THAT(DynamicArray(DataTypeFor<TypeParam>(), {1, 2, 0}), IsEmpty());
  }
}

TYPED_TEST(DynamicArrayTest, RankZero) {
  const std::vector<int64_t> extents = {};

  DynamicArray a(DataTypeFor<TypeParam>(), extents);
  EXPECT_EQ(a.data_type(), DataTypeFor<TypeParam>());
  EXPECT_EQ(a.rank(), 0);
  EXPECT_EQ(a.NumElements(), 1);
  EXPECT_FALSE(a.empty());
  EXPECT_EQ(a.ElementSizeBytes(), sizeof(TypeParam));
  EXPECT_EQ(a.TotalSizeBytes(), sizeof(TypeParam));

  a.At<TypeParam>({}) = Pattern<TypeParam>({});
  EXPECT_EQ(a.At<TypeParam>({}), Pattern<TypeParam>({}));

  // Copy.
  DynamicArray b = a;
  EXPECT_EQ(b.At<TypeParam>({}), Pattern<TypeParam>({}));

  // Ensure that it's a real copy.
  a.At<TypeParam>({}) = Pattern2<TypeParam>({});           // Change a.
  EXPECT_EQ(b.At<TypeParam>({}), Pattern<TypeParam>({}));  // b unaffected.

  // TODO(jiawen): Add a move test.
  // TODO(jiawen): Add for_each_index then we can loop over Ranks.
}

TYPED_TEST(DynamicArrayTest, RankOne) {
  const std::vector<int64_t> extents = {57};

  DynamicArray a(DataTypeFor<TypeParam>(), extents);
  EXPECT_EQ(a.data_type(), DataTypeFor<TypeParam>());
  EXPECT_EQ(a.rank(), 1);
  EXPECT_EQ(a.NumElements(), extents[0]);
  EXPECT_EQ(a.ElementSizeBytes(), sizeof(TypeParam));
  EXPECT_EQ(a.TotalSizeBytes(), extents[0] * sizeof(TypeParam));

  for (int x = 0; x < extents[0]; ++x) {
    a.Set({x}, Pattern<TypeParam>({x}));
  }

  for (int x = 0; x < extents[0]; ++x) {
    EXPECT_EQ(a.At<TypeParam>({x}), Pattern<TypeParam>({x}));
  }
}

TYPED_TEST(DynamicArrayTest, RankTwo) {
  const std::vector<int64_t> extents = {57, 43};

  DynamicArray a(DataTypeFor<TypeParam>(), extents);
  EXPECT_EQ(a.data_type(), DataTypeFor<TypeParam>());
  EXPECT_EQ(a.rank(), 2);
  EXPECT_EQ(a.NumElements(), extents[0] * extents[1]);
  EXPECT_EQ(a.ElementSizeBytes(), sizeof(TypeParam));
  EXPECT_EQ(a.TotalSizeBytes(), extents[0] * extents[1] * sizeof(TypeParam));

  for (int y = 0; y < extents[1]; ++y) {
    for (int x = 0; x < extents[0]; ++x) {
      a.Set({x, y}, Pattern<TypeParam>({x, y}));
    }
  }

  for (int y = 0; y < extents[1]; ++y) {
    for (int x = 0; x < extents[0]; ++x) {
      EXPECT_EQ(a.At<TypeParam>({x, y}), Pattern<TypeParam>({x, y}));
    }
  }
}

TYPED_TEST(DynamicArrayTest, RankThree) {
  const std::vector<int64_t> extents = {57, 43, 23};

  DynamicArray a(DataTypeFor<TypeParam>(), extents);
  EXPECT_EQ(a.data_type(), DataTypeFor<TypeParam>());
  EXPECT_EQ(a.rank(), 3);
  EXPECT_EQ(a.NumElements(), extents[0] * extents[1] * extents[2]);
  EXPECT_EQ(a.ElementSizeBytes(), sizeof(TypeParam));
  EXPECT_EQ(a.TotalSizeBytes(),
            extents[0] * extents[1] * extents[2] * sizeof(TypeParam));

  for (int z = 0; z < extents[2]; ++z) {
    for (int y = 0; y < extents[1]; ++y) {
      for (int x = 0; x < extents[0]; ++x) {
        a.Set({x, y, z}, Pattern<TypeParam>({x, y, z}));
      }
    }
  }

  for (int z = 0; z < extents[2]; ++z) {
    for (int y = 0; y < extents[1]; ++y) {
      for (int x = 0; x < extents[0]; ++x) {
        EXPECT_EQ(a.At<TypeParam>({x, y, z}), Pattern<TypeParam>({x, y, z}));
      }
    }
  }
}

TYPED_TEST(DynamicArrayTest, DynamicArrayRefRef_RankOne) {
  const std::vector<int64_t> extents = {57};

  DynamicArray a(DataTypeFor<TypeParam>(), extents);

  DynamicArrayRef dar = a.ref();

  EXPECT_EQ(dar.data_type(), DataTypeFor<TypeParam>());
  EXPECT_EQ(dar.rank(), 1);
  EXPECT_EQ(dar.NumElements(), extents[0]);
  EXPECT_EQ(dar.ElementSizeBytes(), sizeof(TypeParam));
  EXPECT_EQ(dar.TotalSizeBytes(), extents[0] * sizeof(TypeParam));

  for (int x = 0; x < extents[0]; ++x) {
    a.Set({x}, Pattern<TypeParam>({x}));
  }

  for (int x = 0; x < extents[0]; ++x) {
    EXPECT_EQ(a.At<TypeParam>({x}), Pattern<TypeParam>({x}));
  }

  // TODO(jiawen): More tests. Make another view from the original array. Check
  // that mutating one view is reflected in the other.
}

TYPED_TEST(DynamicArrayTest, ArrayRefOf_RankZero) {
  constexpr size_t kRank = 0;
  const std::vector<int64_t> extents = {};

  DynamicArray a(DataTypeFor<TypeParam>(), extents);

  const nda::const_array_ref_of_rank<TypeParam, kRank> ar =
      ArrayRefOf<const TypeParam, kRank>(a);

  a.At<TypeParam>({}) = TypeParam(42);
  EXPECT_EQ(ar(), TypeParam(42));
}

// TODO(jiawen): More tests: const and non-const.
TYPED_TEST(DynamicArrayTest, ArrayRefOf_RankTwo) {
  constexpr size_t kRank = 2;
  const std::vector<int64_t> extents = {57, 43};

  DynamicArray a(DataTypeFor<TypeParam>(), extents);

  const nda::const_array_ref_of_rank<TypeParam, kRank> ar =
      ArrayRefOf<const TypeParam, kRank>(a);

  EXPECT_EQ(ar.width(), extents[0]);
  EXPECT_EQ(ar.height(), extents[1]);

  for (int y = 0; y < extents[1]; ++y) {
    for (int x = 0; x < extents[0]; ++x) {
      a.At<TypeParam>({x, y}) = Pattern<TypeParam>({x, y});
    }
  }

  for (auto y : ar.y()) {
    for (auto x : ar.x()) {
      EXPECT_EQ(ar(x, y), Pattern<TypeParam>({x, y}));
    }
  }
}

// TODO(jiawen): More tests: const and non-const.
TYPED_TEST(DynamicArrayTest, ArrayRefOf_RankThree) {
  constexpr size_t kRank = 3;
  const std::vector<int64_t> extents = {57, 43, 23};

  DynamicArray a(DataTypeFor<TypeParam>(), extents);

  const nda::const_array_ref_of_rank<TypeParam, kRank> ar =
      ArrayRefOf<const TypeParam, kRank>(a);

  EXPECT_EQ(ar.width(), extents[0]);
  EXPECT_EQ(ar.height(), extents[1]);
  EXPECT_EQ(ar.channels(), extents[2]);

  for (int z = 0; z < extents[2]; ++z) {
    for (int y = 0; y < extents[1]; ++y) {
      for (int x = 0; x < extents[0]; ++x) {
        a.At<TypeParam>({x, y, z}) = Pattern<TypeParam>({x, y, z});
      }
    }
  }

  for (auto z : ar.z()) {
    for (auto y : ar.y()) {
      for (auto x : ar.x()) {
        EXPECT_EQ(ar(x, y, z), Pattern<TypeParam>({x, y, z}));
      }
    }
  }
}

TYPED_TEST(DynamicArrayTest, DynamicArrayRefOf_RankZero) {
  constexpr size_t Rank = 0;
  nda::array_of_rank<TypeParam, 0> ar;

  DynamicArrayRef dar = DynamicArrayRefOf<TypeParam, Rank>(ar.ref());

  EXPECT_EQ(ar.rank(), 0);

  ar() = TypeParam(42);
  EXPECT_EQ(dar.At<TypeParam>({}), TypeParam(42));
}

TYPED_TEST(DynamicArrayTest, DynamicArrayRefOf_RankThree) {
  constexpr size_t Rank = 3;
  nda::array_of_rank<TypeParam, 3> ar({57, 43, 23});

  DynamicArrayRef dar = DynamicArrayRefOf<TypeParam, Rank>(ar.ref());

  EXPECT_EQ(dar.rank(), 3);
  EXPECT_THAT(dar.width(), 57);
  EXPECT_THAT(dar.height(), 43);
  EXPECT_THAT(dar.channels(), 23);

  for (auto z : ar.z()) {
    for (auto y : ar.y()) {
      for (auto x : ar.x()) {
        ar(x, y, z) = Pattern<TypeParam>({x, y, z});
      }
    }
  }

  for (auto z : ar.z()) {
    for (auto y : ar.y()) {
      for (auto x : ar.x()) {
        EXPECT_EQ(dar.At<TypeParam>({x, y, z}), Pattern<TypeParam>({x, y, z}));
      }
    }
  }
}

}  // namespace
}  // namespace npy_array
