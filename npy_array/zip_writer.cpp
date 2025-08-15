#include "npy_array/zip_writer.h"

#include <utility>

#include "absl/log/check.h"
#include "absl/strings/str_cat.h"
#include "third_party/minizip-ng/mz.h"
#include "third_party/minizip-ng/mz_strm.h"
#include "third_party/minizip-ng/mz_strm_mem.h"
#include "third_party/minizip-ng/mz_zip.h"
#include "third_party/minizip-ng/mz_zip_rw.h"

namespace npy_array {

ZipWriter::ZipWriter(size_t memory_grow_size)
    : mem_stream_(mz_stream_mem_create()), zip_writer_(mz_zip_writer_create()) {
  CHECK_NE(mem_stream_, nullptr);
  CHECK_NE(zip_writer_, nullptr);

  mz_stream_mem_set_grow_size(mem_stream_, memory_grow_size);

  // CHECK these too.
  int32_t err;

  err = mz_stream_open(mem_stream_, /*path=*/nullptr, MZ_OPEN_MODE_CREATE);
  CHECK_EQ(err, MZ_OK);

  err = mz_zip_writer_open(zip_writer_, mem_stream_, /*append=*/0);
  CHECK_EQ(err, MZ_OK);
}

ZipWriter::ZipWriter(ZipWriter&& other) { *this = std::move(other); }

ZipWriter& ZipWriter::operator=(ZipWriter&& other) {
  if (this != &other) {
    [[maybe_unused]] absl::Status _ = Close(/*compressed_data=*/nullptr);
    mem_stream_ = std::exchange(other.mem_stream_, nullptr);
    zip_writer_ = std::exchange(other.zip_writer_, nullptr);
  }
  return *this;
}

ZipWriter::~ZipWriter() {
  [[maybe_unused]] absl::Status _ = Close(/*compressed_data=*/nullptr);
}

absl::Status ZipWriter::AddFile(const std::filesystem::path& path,
                                std::string_view data) {
  return AddFile(path, data, /*options=*/{});
}

absl::Status ZipWriter::AddFile(const std::filesystem::path& path,
                                std::string_view data,
                                const AddFileOptions& options) {
  // The compression method (STORE vs DEFLATE) must be stored in `file_info`. We
  // can call `mz_zip_writer_set_compress_method` but `file_info` has
  // precedence.

  // In contrast, compression level is not stored in `file_info`. We must call
  // `mz_zip_writer_set_compress_level` to set it.
  mz_zip_writer_set_compress_level(zip_writer_, options.level);

  mz_zip_file file_info = {};
  file_info.filename = path.c_str();
  file_info.compression_method = (options.method == ZipMethod::kStore)
                                     ? MZ_COMPRESS_METHOD_STORE
                                     : MZ_COMPRESS_METHOD_DEFLATE;
  file_info.flag = MZ_ZIP_FLAG_UTF8;  // Filenames are UTF-8.

  const int32_t err = mz_zip_writer_add_buffer(
      zip_writer_, const_cast<char*>(data.data()), data.size(), &file_info);
  if (err != MZ_OK) {
    return absl::InternalError(
        absl::StrCat("mz_zip_writer_add_buffer failed, err = ", err));
  }

  return absl::OkStatus();
}

absl::StatusOr<std::string> ZipWriter::Close() && {
  std::string compressed_data;
  absl::Status status = Close(&compressed_data);
  if (!status.ok()) {
    return status;
  }
  return std::move(compressed_data);
}

absl::Status ZipWriter::Close(std::string* compressed_data) {
  if (zip_writer_ == nullptr) {
    return absl::InternalError("zip file is already closed");
  }
  if (mem_stream_ == nullptr) {
    return absl::InternalError("zip file is already closed");
  }

  int32_t err;

  err = mz_zip_writer_close(zip_writer_);
  if (err != MZ_OK) {
    return absl::InternalError(
        absl::StrCat("mz_zip_writer_close failed, err = ", err));
  }
  mz_zip_writer_delete(&zip_writer_);

  err = mz_stream_close(mem_stream_);
  if (err != MZ_OK) {
    return absl::InternalError(
        absl::StrCat("mz_stream_close failed, err = ", err));
  }

  // Copy out compressed data if requested.
  // TODO(jiawen): Ideally, the memory stream would be backed directly by the
  // std::string.
  if (compressed_data != nullptr) {
    int32_t length;
    mz_stream_mem_get_buffer_length(mem_stream_, &length);

    *compressed_data = std::string(length, '\0');
    err = mz_stream_mem_seek(mem_stream_, 0, /*origin=*/MZ_SEEK_SET);
    if (err != MZ_OK) {
      return absl::InternalError(
          absl::StrCat("mz_stream_mem_seek failed, err = ", err));
    }

    int32_t bytes_read =
        mz_stream_mem_read(mem_stream_, compressed_data->data(), length);
    if (bytes_read != length) {
      return absl::InternalError(
          absl::StrCat("mz_stream_mem_read failed, bytes_read = ", bytes_read,
                       ", length = ", length));
    }
  }

  mz_stream_mem_delete(&mem_stream_);

  return absl::OkStatus();
}

}  // namespace npy_array
