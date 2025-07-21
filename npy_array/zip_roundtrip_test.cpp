#include <string>

#include "absl/status/status_matchers.h"
#include "absl/strings/str_cat.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "npy_array/zip_reader.h"
#include "npy_array/zip_writer.h"

using ::absl_testing::IsOk;
using ::testing::Eq;
using ::testing::Not;
using ::testing::SizeIs;
using ::testing::TempDir;

namespace npy_array {
namespace {

TEST(ZipRoundtripTest, CanWriteZipFileWithNoContents) {
  const std::string filename = absl::StrCat(TempDir(), "test_empty.zip");

  {
    absl::StatusOr<ZipWriter> maybe_zip_writer = ZipWriter::open(filename);
    EXPECT_THAT(maybe_zip_writer, IsOk());

    // Destructor closes.
  }

  // The zip file exists but has no contents.
  EXPECT_TRUE(std::filesystem::exists(filename));

  // Apparently minizip can't unzip empty files.
  absl::StatusOr<absl::flat_hash_map<std::string, std::string>> maybe_contents =
      readZipFile(filename);
  EXPECT_THAT(maybe_contents, Not(IsOk()));
}

TEST(ZipRoundtripTest, CanWriteMultipleFiles) {
  const std::string filename = absl::StrCat(TempDir(), "test_multiple.zip");
  {
    absl::StatusOr<ZipWriter> maybe_zip_writer = ZipWriter::open(filename);
    EXPECT_THAT(maybe_zip_writer, IsOk());

    EXPECT_THAT(maybe_zip_writer->addFile("hello.txt", "hello"), IsOk());
    EXPECT_THAT(maybe_zip_writer->addFile("world.txt", "world"), IsOk());

    // Explicit close.
    EXPECT_THAT(maybe_zip_writer->close(), IsOk());
    EXPECT_TRUE(maybe_zip_writer->isClosed());
  }

  {
    absl::StatusOr<absl::flat_hash_map<std::string, std::string>>
        maybe_contents = readZipFile(filename);
    EXPECT_THAT(maybe_contents, IsOk());

    EXPECT_THAT(*maybe_contents, SizeIs(2));
    EXPECT_THAT(maybe_contents->at("hello.txt"), Eq("hello"));
    EXPECT_THAT(maybe_contents->at("world.txt"), Eq("world"));
  }
}

TEST(ZipRoundtripTest, CanWriteMultipleFiles_LastOneWins) {
  const std::string filename =
      absl::StrCat(TempDir(), "test_multiple_repeated.zip");
  {
    absl::StatusOr<ZipWriter> maybe_zip_writer = ZipWriter::open(filename);
    EXPECT_THAT(maybe_zip_writer, IsOk());

    EXPECT_THAT(maybe_zip_writer->addFile("hello.txt", "hello"), IsOk());
    EXPECT_THAT(maybe_zip_writer->addFile("world.txt", "world"), IsOk());
    EXPECT_THAT(maybe_zip_writer->addFile("world.txt", "world2"), IsOk());

    // Destructor closes.
  }

  {
    absl::StatusOr<absl::flat_hash_map<std::string, std::string>>
        maybe_contents = readZipFile(filename);
    EXPECT_THAT(maybe_contents, IsOk());

    EXPECT_THAT(*maybe_contents, SizeIs(2));
    EXPECT_THAT(maybe_contents->at("hello.txt"), Eq("hello"));
    EXPECT_THAT(maybe_contents->at("world.txt"), Eq("world2"));
  }
}

}  // namespace
}  // namespace npy_array
