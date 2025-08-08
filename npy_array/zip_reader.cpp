#include "npy_array/zip_reader.h"

#include <utility>

#include "absl/cleanup/cleanup.h"
#include "absl/strings/str_cat.h"
#include "third_party/minizip-ng/mz.h"
#include "third_party/minizip-ng/mz_strm.h"
#include "third_party/minizip-ng/mz_zip.h"
#include "third_party/minizip-ng/mz_zip_rw.h"

namespace npy_array {

absl::StatusOr<absl::flat_hash_map<std::filesystem::path, std::string>>
ReadZipFile(std::string_view data) {
  void* zip_reader = mz_zip_reader_create();
  if (zip_reader == nullptr) {
    return absl::InternalError("mz_zip_reader_create failed");
  }
  absl::Cleanup cleanup_zip_reader([&] { mz_zip_reader_delete(&zip_reader); });

  int32_t err;
  absl::flat_hash_map<std::filesystem::path, std::string> result;

  err = mz_zip_reader_open_buffer(
      zip_reader,
      const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(data.data())),
      data.size(), /*copy=*/0);
  if (err != MZ_OK) {
    return absl::InternalError(
        absl::StrCat("mz_zip_reader_open_buffer failed, err = ", err));
  }
  absl::Cleanup cleanup_zip_reader_open_buffer(
      [&] { mz_zip_reader_close(zip_reader); });

  err = mz_zip_reader_goto_first_entry(zip_reader);
  // If the zip file is empty, `mz_zip_reader_goto_first_entry` returns -100.
  // This is not an error.
  if (err == MZ_END_OF_LIST) {
    return result;
  }

  if (err != MZ_OK) {
    return absl::InternalError(
        absl::StrCat("mz_zip_reader_goto_first_entry failed, err = ", err));
  }

  do {
    err = mz_zip_reader_entry_open(zip_reader);
    if (err != MZ_OK) {
      return absl::InternalError(
          absl::StrCat("mz_zip_reader_entry_open failed, err = ", err));
    }

    mz_zip_file* file_info = nullptr;
    err = mz_zip_reader_entry_get_info(zip_reader, &file_info);
    if (err != MZ_OK) {
      return absl::InternalError(
          absl::StrCat("mz_zip_reader_entry_get_info failed, err = ", err));
    }

    const int32_t buffer_length = file_info->uncompressed_size;

    std::filesystem::path path(file_info->filename);
    std::string data(buffer_length, '\0');
    err =
        mz_zip_reader_entry_save_buffer(zip_reader, data.data(), buffer_length);
    if (err != MZ_OK) {
      return absl::InternalError(
          absl::StrCat("mz_zip_reader_entry_save_buffer failed, err = ", err));
    }

    result[path] = std::move(data);

    // Apparently I should not call `mz_zip_reader_entry_close`.
    // `mz_zip_reader_goto_next_entry` will close the entry for us.
  } while (mz_zip_reader_goto_next_entry(zip_reader) == MZ_OK);

  return result;
}

}  // namespace npy_array
