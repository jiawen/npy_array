#include "npy_array/zip_reader.h"

#include <utility>

#include "absl/strings/str_cat.h"
#include "absl/types/span.h"
#include "npy_array/status_macros.h"
#include "third_party/zlib/minizip/unzip.h"

namespace npy_array {

namespace {

absl::StatusOr<std::string> GetCurrentFilename(unzFile unz_file) {
  unz_file_info file_info;
  memset(&file_info, 0, sizeof(unz_file_info));

  int ret;

  ret = unzGetCurrentFileInfo(unz_file, &file_info, NULL, 0, NULL, 0, NULL, 0);
  if (ret != UNZ_OK) {
    return absl::InternalError(
        "unzGetCurrentFileInfo failed retrieving filename size");
  }

  std::string filename(file_info.size_filename, '\0');
  ret = unzGetCurrentFileInfo(unz_file, &file_info, filename.data(),
                              file_info.size_filename + 1, NULL, 0, NULL, 0);
  if (ret != UNZ_OK) {
    return absl::InternalError(
        "unzGetCurrentFileInfo failed retrieving filename");
  }

  return filename;
}

absl::StatusOr<std::string> ReadCurrentFile(unzFile unz_file,
                                            absl::Span<char> buffer) {
  std::string result;

  // unzReadCurrentFile returns:
  // -1 on error.
  // 0 on EOF.
  // > 0 on success.
  int bytes_read;
  while ((bytes_read =
              unzReadCurrentFile(unz_file, buffer.data(), buffer.size())) > 0) {
    // Read a positive number of bytes.
    std::string_view valid_data(reinterpret_cast<char*>(buffer.data()),
                                bytes_read);
    absl::StrAppend(&result, valid_data);
  }

  if (bytes_read < 0) {
    return absl::InternalError("unzReadCurrentFile failed");
  }

  return result;
}

}  // namespace

absl::StatusOr<absl::flat_hash_map<std::string, std::string>> ReadZipFile(
    const std::filesystem::path& path, const ReadZipOptions& options) {
  unzFile unz_file = unzOpen64(path.c_str());
  if (unz_file == NULL) {
    return absl::InternalError(
        absl::StrCat("unzOpen64 failed opening", path.c_str(), " for read"));
  }

  int ret;
  absl::flat_hash_map<std::string, std::string> result;

  // Allocate a fixed size buffer.
  std::string buffer(options.buffer_size, '\0');
  // Make a view of it to pass to readCurrentFile().
  absl::Span<char> buffer_span(buffer);

  ret = unzGoToFirstFile(unz_file);
  if (ret != UNZ_OK) {
    return absl::InternalError("unzGoToFirstFile failed");
  }

  do {
    ret = unzOpenCurrentFile(unz_file);
    if (ret != UNZ_OK) {
      return absl::InternalError("unzOpenCurrentFile failed");
    }

    std::string filename;
    ASSIGN_OR_RETURN(filename, GetCurrentFilename(unz_file));

    std::string data;
    ASSIGN_OR_RETURN(data, ReadCurrentFile(unz_file, buffer_span));

    result[filename] = std::move(data);

    ret = unzCloseCurrentFile(unz_file);
    if (ret != UNZ_OK) {
      return absl::InternalError("unzCloseCurrentFile failed (bad CRC)");
    }
  } while (unzGoToNextFile(unz_file) == UNZ_OK);

  return result;
}

}  // namespace npy_array
