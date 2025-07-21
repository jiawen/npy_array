#ifndef NPY_ARRAY_ZIP_READER_H_
#define NPY_ARRAY_ZIP_READER_H_

#include <filesystem>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

namespace npy_array {

struct ReadZipOptions {
  static constexpr int kDefaultBufferSize = 16384;  // 16 kB.

  int buffer_size = kDefaultBufferSize;

  // TODO(jiawen): Add an option to attempt to continue if parts are corrupt.
};

// Reads the entire contents of a zip archive at `path` into a std::string ->
// std::string map.
//
// ! Beware of zip bombs. This convenient function reads everything but does not
// limit how much memory it uses.
// ! Keys (filenames) in a zip file may not be unique (i.e., it is malformed).
// In this case, the last entry wins.
// - If any part of the reading process fails, returns an error instead of
// partially read data.
absl::StatusOr<absl::flat_hash_map<std::string, std::string>> readZipFile(
    const std::filesystem::path& path, const ReadZipOptions& options = {});

// TODO(jiawen): Add a streaming API if we ever need one. It is not difficult
// since we read in chunks. But the user probably doesn't want chunks. It's
// probably better to stream strings.

}  // namespace npy_array

#endif  // NPY_ARRAY_ZIP_READER_H_
