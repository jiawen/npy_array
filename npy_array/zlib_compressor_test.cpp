#include "npy_array/zlib_compressor.h"

#include <bit>
#include <string>
#include <string_view>

#include "absl/status/status_matchers.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::absl_testing::IsOk;

namespace npy_array {
namespace {

TEST(DeflateTest, EmptyStringAndHeader) {
  const std::string src = "";
  absl::StatusOr<std::string> compressed = ZlibCompress(src);
  EXPECT_THAT(compressed, IsOk());

  EXPECT_EQ(compressed->size(), 8);

  // The zlib header is 2 bytes.
  // - The first byte (CMF) is almost always 0x78.
  //   - Bits [3:0]: compression method (8 for DEFLATE).
  //   - Bits [7:4]: window size (7 for 32K).
  // - The second byte (FLG) are the flags.
  //   - Bits [4:0]: check bits such that CHK % 31 == 0, where:
  //   CHK = CMF * 256 + FLG.
  //   - Bit [5]: whether to use a preset dictionary.
  //   - Bit [7:6]: compression level.
  const uint8_t cmf = std::bit_cast<uint8_t>((*compressed)[0]);
  const uint8_t flg = std::bit_cast<uint8_t>((*compressed)[1]);

  EXPECT_EQ(cmf, 0x78);
  EXPECT_EQ(flg, 0x9c);

  const uint16_t chk = cmf * 256 + flg;
  EXPECT_EQ(chk % 31, 0);

  absl::StatusOr<std::string> decompressed =
      ZlibDecompress(*compressed, src.size());
  EXPECT_THAT(decompressed, IsOk());

  EXPECT_EQ(src, *decompressed);
}

TEST(DeflateTest, RoundtripHelloWorld) {
  const std::string src = "Hello, World!";
  absl::StatusOr<std::string> compressed = ZlibCompress(src);
  EXPECT_THAT(compressed, IsOk());

  absl::StatusOr<std::string> decompressed =
      ZlibDecompress(*compressed, src.size());
  EXPECT_THAT(decompressed, IsOk());

  EXPECT_EQ(src, *decompressed);
}

TEST(DeflateTest, CompressZeroes) {
  const std::string src(16 * 1024 * 1024, '\0');  // 16 MB.
  absl::StatusOr<std::string> compressed = ZlibCompress(src);
  EXPECT_THAT(compressed, IsOk());

  EXPECT_EQ(compressed->size(), 16316);  // ~16 KB.

  absl::StatusOr<std::string> decompressed =
      ZlibDecompress(*compressed, src.size());
  EXPECT_THAT(decompressed, IsOk());
  EXPECT_EQ(src, *decompressed);
}

}  // namespace
}  // namespace npy_array
