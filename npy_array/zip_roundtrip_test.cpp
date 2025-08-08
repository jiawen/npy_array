#include <string>

#include "absl/status/status_matchers.h"
#include "absl/strings/str_cat.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "npy_array/zip_reader.h"
#include "npy_array/zip_writer.h"

using ::absl_testing::IsOk;
using ::absl_testing::IsOkAndHolds;
using ::testing::Eq;
using ::testing::Gt;
using ::testing::IsEmpty;
using ::testing::Lt;
using ::testing::SizeIs;

namespace npy_array {
namespace {

TEST(ZipWriterTest, CanUseStore) {
  ZipWriter zip_writer;

  constexpr size_t kDataSize = 1024 * 1024;
  const std::string kData(kDataSize, 'a');  // 1 MB of 'a's.

  EXPECT_THAT(zip_writer.AddFile("hello.txt", kData,
                                 ZipWriter::AddFileOptions{
                                     .method = ZipMethod::kStore,
                                 }),
              IsOk());

  // The stored data is slightly larger than the original data.
  absl::StatusOr<std::string> maybe_data = std::move(zip_writer).Close();
  EXPECT_THAT(maybe_data, IsOkAndHolds(SizeIs(Gt(kData.size()))));
}

TEST(ZipWriterTest, CanUseDeflate) {
  ZipWriter zip_writer_level_1;
  ZipWriter zip_writer_level_9;

  constexpr size_t kDataSize = 1024 * 1024;
  const std::string kData(kDataSize, 'a');  // 1 MB of 'a's.

  EXPECT_THAT(zip_writer_level_1.AddFile("hello.txt", kData,
                                         ZipWriter::AddFileOptions{
                                             .method = ZipMethod::kDeflate,
                                             .level = 1,
                                         }),
              IsOk());

  EXPECT_THAT(zip_writer_level_9.AddFile("hello.txt", kData,
                                         ZipWriter::AddFileOptions{
                                             .method = ZipMethod::kDeflate,
                                             .level = 9,
                                         }),
              IsOk());

  // The compressed data at level 1 is smaller than the original data.
  absl::StatusOr<std::string> maybe_data_level_1 =
      std::move(zip_writer_level_1).Close();
  EXPECT_THAT(maybe_data_level_1, IsOkAndHolds(SizeIs(Lt(kData.size()))));

  // The compressed data at level 9 is smaller than the original data.
  absl::StatusOr<std::string> maybe_data_level_9 =
      std::move(zip_writer_level_9).Close();
  EXPECT_THAT(maybe_data_level_9, IsOkAndHolds(SizeIs(Lt(kData.size()))));

  // The compressed data at level 9 is smaller than the compressed data at level
  // 1.
  EXPECT_LT(maybe_data_level_9->size(), maybe_data_level_1->size());
}

TEST(ZipRoundtripTest, CanWriteZipFileWithNoContents) {
  ZipWriter zip_writer;
  absl::StatusOr<std::string> maybe_data = std::move(zip_writer).Close();

  EXPECT_THAT(maybe_data, IsOk());

  absl::StatusOr<absl::flat_hash_map<std::filesystem::path, std::string>>
      maybe_contents = ReadZipFile(*maybe_data);
  EXPECT_THAT(maybe_contents, IsOkAndHolds(IsEmpty()));
}

TEST(ZipRoundtripTest, CanWriteMultipleFiles) {
  ZipWriter zip_writer;
  EXPECT_THAT(zip_writer.AddFile("hello.txt", "hello"), IsOk());
  EXPECT_THAT(zip_writer.AddFile("world.txt", "world"), IsOk());
  absl::StatusOr<std::string> maybe_data = std::move(zip_writer).Close();
  EXPECT_THAT(maybe_data, IsOk());

  absl::StatusOr<absl::flat_hash_map<std::filesystem::path, std::string>>
      maybe_contents = ReadZipFile(*maybe_data);
  EXPECT_THAT(maybe_contents, IsOk());
  EXPECT_THAT(*maybe_contents, SizeIs(2));
  EXPECT_THAT(maybe_contents->at("hello.txt"), Eq("hello"));
  EXPECT_THAT(maybe_contents->at("world.txt"), Eq("world"));
}

TEST(ZipRoundtripTest, CanWriteMultipleFiles_LastOneWins) {
  ZipWriter zip_writer;
  EXPECT_THAT(zip_writer.AddFile("hello.txt", "hello"), IsOk());
  EXPECT_THAT(zip_writer.AddFile("world.txt", "world"), IsOk());
  EXPECT_THAT(zip_writer.AddFile("world.txt", "world2"), IsOk());

  absl::StatusOr<std::string> maybe_data = std::move(zip_writer).Close();
  EXPECT_THAT(maybe_data, IsOk());

  absl::StatusOr<absl::flat_hash_map<std::filesystem::path, std::string>>
      maybe_contents = ReadZipFile(*maybe_data);
  EXPECT_THAT(maybe_contents, IsOk());
  EXPECT_THAT(*maybe_contents, SizeIs(2));
  EXPECT_THAT(maybe_contents->at("hello.txt"), Eq("hello"));
  EXPECT_THAT(maybe_contents->at("world.txt"), Eq("world2"));
}

}  // namespace
}  // namespace npy_array
