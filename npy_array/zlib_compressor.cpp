#include "npy_array/zlib_compressor.h"

#include <cstdint>

#include "absl/strings/str_cat.h"
#include "third_party/zlib-ng/zlib-ng.h"

namespace npy_array {

absl::StatusOr<std::string> ZlibCompress(std::string_view src) {
  const size_t dst_bound = zng_compressBound(src.size());
  std::string dst(dst_bound, '\0');

  size_t dst_size = dst.size();
  const int32_t err =
      zng_compress(reinterpret_cast<uint8_t*>(dst.data()), &dst_size,
                   reinterpret_cast<const uint8_t*>(src.data()), src.size());
  if (err != Z_OK) {
    return absl::InternalError(
        absl::StrCat("zlib compress failed with error: ", err));
  }

  dst.resize(dst_size);
  return dst;
}

absl::StatusOr<std::string> ZlibDecompress(std::string_view src,
                                           size_t uncompressed_size) {
  std::string dst(uncompressed_size, '\0');

  size_t dst_size = dst.size();
  const int32_t err =
      zng_uncompress(reinterpret_cast<uint8_t*>(dst.data()), &dst_size,
                     reinterpret_cast<const uint8_t*>(src.data()), src.size());
  // Z_MEM_ERROR: not enough memory
  // Z_BUF_ERROR: not enough room in the output buffer
  // Z_DATA_ERROR: input data was corrupted
  if (err != Z_OK) {
    return absl::InternalError(
        absl::StrCat("zlib uncompress failed with error: ", err));
  }

  dst.resize(dst_size);
  return dst;
}

}  // namespace npy_array
