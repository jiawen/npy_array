#include "npy_array/npy_dynamic_array.h"

#include "array/array.h"
#include "gtest/gtest.h"
#include "npy_array/gtest_half.h"
#include "npy_array/npy_array.h"
#include "npy_array/zip_reader.h"

namespace npy_array {
namespace {

constexpr std::string_view kNpzPath =
    "tests/data/npy_dynamic_array_testdata.npz";

template <typename T>
T GetDynamicArrayOrRefFromNpy(std::string& npy_data) {
  absl::StatusOr<T> arr_or_ref;
  if constexpr (std::is_same_v<T, DynamicArray>) {
    arr_or_ref = DecodeDynamicArrayFromNpy(npy_data);
  } else {
    arr_or_ref = MakeDynamicArrayRefOfNpy(npy_data);
  }
  EXPECT_TRUE(arr_or_ref.ok());
  return *std::move(arr_or_ref);
}

template <typename T, typename ArrayOrRefType>
void TestReadFromNpz() {
  auto npz_data = ReadZipFile(kNpzPath);
  EXPECT_TRUE(npz_data.ok());

  DataType data_type = DataTypeFor<T>();

  {
    // Empty array
    std::string empty_filename = absl::StrCat(data_type, "_empty.npy");
    auto it = npz_data->find(empty_filename);
    EXPECT_NE(it, npz_data->end()) << "File not found: " << empty_filename;
    EXPECT_FALSE(it->second.empty()) << "File is empty: " << empty_filename;

    ArrayOrRefType arr =
        GetDynamicArrayOrRefFromNpy<ArrayOrRefType>(it->second);

    EXPECT_EQ(arr.data_type(), data_type);
    EXPECT_TRUE(arr.empty()) << "Array should be empty: " << empty_filename;
  }

  for (int rank = 0; rank < 6; rank++) {
    std::string filename = absl::StrCat(data_type, "_rank", rank, ".npy");
    auto it = npz_data->find(filename);
    EXPECT_NE(it, npz_data->end()) << "File not found: " << filename;
    EXPECT_FALSE(it->second.empty()) << "File is empty: " << filename;

    ArrayOrRefType arr =
        GetDynamicArrayOrRefFromNpy<ArrayOrRefType>(it->second);

    EXPECT_EQ(arr.data_type(), data_type);
    EXPECT_FALSE(arr.empty());
    EXPECT_EQ(arr.rank(), rank) << "Rank mismatch for file: " << filename;

    const DynamicShape& shape = arr.shape();
    EXPECT_EQ(shape.rank(), rank);

    if (rank == 0) {
      EXPECT_FALSE(shape.empty());
      EXPECT_EQ(shape.size(), 1);

      EXPECT_EQ(arr.NumElements(), 1);
      EXPECT_EQ(arr.template At<T>({}), 0);
    } else {
      // Shape should be [1, 2, ..., N] for rank N.
      for (int d = 0; d < rank; d++) {
        EXPECT_EQ(shape.min(d), 0);
        EXPECT_EQ(shape.extent(d), rank - d);
      }

      // Check if array values are parsed correctly:
      // arr(i, j, k, ...) = i + j + k + ...
      std::vector<int64_t> indices(rank);
      for (int flat_idx = 0; flat_idx < arr.NumElements(); flat_idx++) {
        T expected = 0;
        for (int d = 0; d < rank; d++) {
          indices[d] = (flat_idx / shape.stride(d)) % shape.extent(d);
          expected += indices[d];
        }
        EXPECT_EQ(arr.template At<T>(indices), expected);
      }
    }
  }
}

template <typename T>
class NpyDynamicArrayTest : public testing::Test {};

// TODO(yutyang, jiawen): npy_array cannot find type for int8 and uint8,
// should figure out why.
using MyTypes = testing::Types<int16_t, int32_t, int64_t, uint16_t, uint32_t,
                               uint64_t, half, float, double>;
TYPED_TEST_SUITE(NpyDynamicArrayTest, MyTypes);

TYPED_TEST(NpyDynamicArrayTest, ReadDynamicArrayFromNpz) {
  TestReadFromNpz<TypeParam, DynamicArray>();
}

TYPED_TEST(NpyDynamicArrayTest, ReadDynamicArrayRefFromNpz) {
  TestReadFromNpz<TypeParam, DynamicArrayRef>();
}

}  // namespace
}  // namespace npy_array
