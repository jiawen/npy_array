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
using ::testing::IsEmpty;
using ::testing::SizeIs;

namespace npy_array {
namespace {

#if 0
TEST(ZipRoundtripTest, Foo) {
  ZipWriter zip_writer;
  EXPECT_THAT(zip_writer.AddFile("hello.txt", "hello",
  EXPECT_THAT(zip_writer.AddFile("dir/world.txt", data,
                                 ZipWriter::AddFileOptions{
                                     .method = ZipMethod::kDeflate,
                                     .level = 9,
                                 }),
              IsOk());

  absl::StatusOr<std::string> maybe_data = std::move(zip_writer).Close();

  EXPECT_THAT(maybe_data, IsOk());

  absl::StatusOr<absl::flat_hash_map<std::filesystem::path, std::string>>
      maybe_contents = ReadZipFile(*maybe_data);
  EXPECT_THAT(maybe_contents, IsOk());
  EXPECT_THAT(maybe_contents->size(), Eq(2));
  EXPECT_THAT(maybe_contents->at("hello.txt"), Eq("hello"));
  EXPECT_THAT(maybe_contents->at("dir/world.txt"), Eq(data));
}
#endif

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
