#ifndef NPY_ARRAY_ZIP_WRITER_H_
#define NPY_ARRAY_ZIP_WRITER_H_

#include <filesystem>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "third_party/zlib/minizip/zip.h"

namespace npy_array {

enum class ZipMethod {
  kStore = 0,    // No compression.
  kDeflate = 1,  // Use the DEFLATE algorithm.
};

class ZipWriter {
 public:
  struct AddFileOptions {
    ZipMethod method = ZipMethod::kDeflate;

    // Compression level.
    // 0: no compression.
    // 1: best speed.
    // 9: best compression.
    // -1: default.
    int level = -1;
  };

  static absl::StatusOr<ZipWriter> Open(const std::filesystem::path& path);

  ZipWriter(ZipWriter&& other);
  ZipWriter& operator=(ZipWriter&& other);

  // Automatically calls `close()`.
  ~ZipWriter();

  // Same as addFile with the default options.
  absl::Status AddFile(const std::filesystem::path& path,
                       std::string_view data);

  // Adds `data` to the zip file at `path`.
  //
  // Does nothing if this ZipWriter is already closed.
  absl::Status AddFile(const std::filesystem::path& path, std::string_view data,
                       const AddFileOptions& options);

  // Explicitly closes this ZipWriter. It can be called multiple times. Once a
  // ZipWriter is closed, it cannot be used to add more files.
  //
  // You may want to do this rather than use the destructor if you want to know
  // if `close()` failed (i.e., if a file is successfully written).
  absl::Status Close();

  // Returns true if `close()` has been called.
  bool IsClosed() const;

 private:
  explicit ZipWriter(zipFile zip_file);

  zipFile zip_file_ = NULL;
};

}  // namespace npy_array

#endif  // NPY_ARRAY_ZIP_WRITER_H_
