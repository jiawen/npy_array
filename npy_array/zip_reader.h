#ifndef NPY_ARRAY_ZIP_READER_H_
#define NPY_ARRAY_ZIP_READER_H_

#include <filesystem>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"

namespace npy_array {

// Reads the entire contents of a zip archive at `data` into a
// std::filesystem::path -> std::string map.
//
// ! Beware of zip bombs. This convenient function reads everything but does not
// limit how much memory it uses.
// ! Keys (filenames) in a zip file may not be unique (i.e., it is malformed).
// In this case, the last entry wins.
// - If any part of the reading process fails, returns an error instead of
// partially read data.
absl::StatusOr<absl::flat_hash_map<std::filesystem::path, std::string>>
ReadZipFile(std::string_view data);

// TODO(jiawen): Add a streaming API if we ever need one. At the granularity of
// files is fine.

}  // namespace npy_array

#endif  // NPY_ARRAY_ZIP_READER_H_
