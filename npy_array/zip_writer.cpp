#include "npy_array/zip_writer.h"

#include <utility>

#include "absl/strings/str_cat.h"

namespace npy_array {

absl::StatusOr<ZipWriter> ZipWriter::open(const std::filesystem::path& path) {
  zipFile zip_file = zipOpen64(path.c_str(), /*append=*/0);
  if (zip_file == NULL) {
    return absl::InternalError(
        absl::StrCat("zipOpen64 failed opening", path.c_str(), " for write"));
  }

  return ZipWriter(zip_file);
}

ZipWriter::ZipWriter(ZipWriter&& other) { *this = std::move(other); }

ZipWriter& ZipWriter::operator=(ZipWriter&& other) {
  if (this != &other) {
    (void)(close());
    zip_file_ = std::exchange(other.zip_file_, (zipFile)NULL);
  }
  return *this;
}

ZipWriter::~ZipWriter() { (void)(close()); }

absl::Status ZipWriter::addFile(const std::filesystem::path& path,
                                std::string_view data) {
  return addFile(path, data, /*options=*/{});
}

absl::Status ZipWriter::addFile(const std::filesystem::path& path,
                                std::string_view data,
                                const AddFileOptions& options) {
  // Always use zip64. If we wanted, can theoreticaly check data.size() and only
  // use zip64 if size > 0xffffffff.
  constexpr int kUseZip64 = 1;

  const int method = (options.method == ZipMethod::kStore) ? 0 : Z_DEFLATED;

  int ret;

  ret = zipOpenNewFileInZip64(
      zip_file_, path.c_str(),
      /*zipfi=*/NULL,
      /*extrafield_local=*/NULL,
      /*size_extrafield_local=*/0, /*extrafield_global=*/NULL,
      /*size_extrafield_global=*/0, /*comment=*/NULL, /*method=*/method,
      /*level=*/options.level, kUseZip64);
  if (ret != ZIP_OK) {
    return absl::InternalError(absl::StrCat(
        "zipOpenNewFileInZip64 failed: failed to open a new file with name ",
        path.c_str(), " within the zip"));
  }

  ret = zipWriteInFileInZip(zip_file_, data.data(), data.size());
  if (ret != ZIP_OK) {
    return absl::InternalError(absl::StrCat(
        "zipWriteInFileInZip failed: failed to write data to file ",
        path.c_str(), " within the zip"));
  }
  ret = zipCloseFileInZip(zip_file_);
  if (ret != ZIP_OK) {
    return absl::InternalError(
        absl::StrCat("zipCloseFileInZip failed: failed to close file ",
                     path.c_str(), " within the zip"));
  }

  return absl::OkStatus();
}

absl::Status ZipWriter::close() {
  if (zip_file_ == NULL) {
    return absl::InternalError("zip file is already closed");
  }

  int ret = zipClose(zip_file_, /*global_comment=*/NULL);
  if (ret != ZIP_OK) {
    return absl::InternalError("zipClose failed");
  }
  zip_file_ = NULL;
  return absl::OkStatus();
}

bool ZipWriter::isClosed() const { return zip_file_ == NULL; }

ZipWriter::ZipWriter(zipFile zip_file) : zip_file_(zip_file) {}

}  // namespace npy_array
