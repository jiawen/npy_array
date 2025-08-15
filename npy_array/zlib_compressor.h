#ifndef NPY_ARRAY_ZLIB_COMPRESSOR_H_
#define NPY_ARRAY_ZLIB_COMPRESSOR_H_

#include <string>
#include <string_view>

#include "absl/status/statusor.h"

namespace npy_array {

// Compresses `src` with the DEFLATE algorithm and returns the compressed data.
// The output includes the zlib header.
absl::StatusOr<std::string> ZlibCompress(std::string_view src);

// Decompresses `src` with the DEFLATE algorithm and returns the decompressed
// data.
//
// Note that the `uncompressed_size` must be known in advance (`src.size()`
// above).
absl::StatusOr<std::string> ZlibDecompress(std::string_view src,
                                           size_t uncompressed_size);

}  // namespace npy_array

#endif  // NPY_ARRAY_ZLIB_COMPRESSOR_H_
