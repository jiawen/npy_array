#ifndef NPY_ARRAY_ZIP_WRITER_H_
#define NPY_ARRAY_ZIP_WRITER_H_

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace npy_array {

enum class ZipMethod {
  kStore = 0,    // No compression.
  kDeflate = 1,  // Use the DEFLATE algorithm.
};

class ZipWriter {
 public:
  // When appending to a zip file, we need to grow the memory buffer. This is
  // the default increment by which it grows.
  static constexpr size_t kDefaultMemoryGrowSize = 1024 * 1024;  // 1 MB.

  struct AddFileOptions {
    ZipMethod method = ZipMethod::kDeflate;

    // Compression level.
    // -1: default.
    // 0: no compression.
    // 1: best speed.
    // 9: best compression.
    int16_t level = -1;

    // TODO(jiawen):
    // - absl::Time or absl::CivilTime.
    // - Permissions?
  };

  // Opens a new ZipWriter that buffers compressed data into memory.
  explicit ZipWriter(size_t memory_grow_size = kDefaultMemoryGrowSize);
  ZipWriter(ZipWriter&& other);
  ZipWriter& operator=(ZipWriter&& other);
  ~ZipWriter();

  // Same as AddFile with the default options.
  absl::Status AddFile(const std::filesystem::path& path,
                       std::string_view data);

  // Adds `data` to the zip file at `path`.
  //
  // Does nothing if this ZipWriter is already closed.
  absl::Status AddFile(const std::filesystem::path& path, std::string_view data,
                       const AddFileOptions& options);

  // Explicitly closes this ZipWriter and returns the compressed data. It can
  // only be used in an rvalue context, e.g.,:
  //
  // absl::StatusOr<std::string> maybe_data = std::move(zip_writer).Close();
  absl::StatusOr<std::string> Close() &&;

 private:
  void* mem_stream_ = nullptr;
  void* zip_writer_ = nullptr;

  // Closes this ZipWriter: no more files can be added. If `compressed_data` is
  // non-null, it will be set to the compressed data.
  absl::Status Close(std::string* compressed_data);
};

}  // namespace npy_array

#endif  // NPY_ARRAY_ZIP_WRITER_H_
