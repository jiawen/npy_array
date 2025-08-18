# TODO(jiawen): put this behind a select().
_APPLE_LOCAL_DEFINES = [
    "_D_DARWIN_C_SOURCE=1",
]

# minizip-ng interface using zlib backend.
# TODO(jiawen): use zlib-ng backend.
cc_library(
    name = "minizip-ng",
    srcs = [
        "mz_crypt.c",
        "mz_crypt.h",
        "mz_os.c",
        "mz_os.h",
        "mz_strm.c",
        "mz_strm_buf.c",
        "mz_strm_buf.h",
        "mz_strm_mem.c",
        "mz_strm_os.h",
        "mz_strm_split.c",
        "mz_strm_split.h",
        "mz_strm_wzaes.h",
        "mz_strm_zlib.c",
        "mz_strm_zlib.h",
        "mz_zip.c",
        "mz_zip_rw.c",
    ] + [
        "mz_os_posix.c",
        "mz_strm_os_posix.c",
    ],  # TODO(jiawen): Add WIN32 support - look at CMakeLists.txt.
    hdrs = [
        "mz.h",
        "mz_strm.h",
        "mz_strm_mem.h",
        "mz_zip.h",
        "mz_zip_rw.h",
    ],
    include_prefix = "third_party/minizip-ng",
    local_defines = [
        "_FILE_OFFSET_BITS=64",
        "HAVE_FSEEKO=1",
        "HAVE_INTTYPES_H=1",
        "HAVE_STDINT_H=1",
        "HAVE_ZLIB=1",
        "MZ_ZIP_NO_CRYPTO=1",
    ] + _APPLE_LOCAL_DEFINES + [
        "_BSD_SOURCE=1",
        "_DEFAULT_SOURCE=1",
        "_POSIX_C_SOURCE=200809L",
    ],  # TODO(jiawen): Add WIN32 support - look at CMakeLists.txt.
    strip_include_prefix = ".",
    visibility = ["//visibility:public"],
    deps = ["@zlib-ng//:zlib-ng"],
)
