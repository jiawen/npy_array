# Copied from https://github.com/protocolbuffers/protobuf/blob/master/third_party/zlib.BUILD
package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # BSD/MIT-like license (for zlib)

_ZLIB_HEADERS = [
    "crc32.h",
    "deflate.h",
    "gzguts.h",
    "inffast.h",
    "inffixed.h",
    "inflate.h",
    "inftrees.h",
    "trees.h",
    "zconf.h",
    "zlib.h",
    "zutil.h",
]

_ZLIB_PREFIXED_HEADERS = ["zlib/include/" + hdr for hdr in _ZLIB_HEADERS]

# In order to limit the damage from the `includes` propagation
# via `:zlib`, copy the public headers to a subdirectory and
# expose those.
genrule(
    name = "copy_public_headers",
    srcs = _ZLIB_HEADERS,
    outs = _ZLIB_PREFIXED_HEADERS,
    cmd = "cp $(SRCS) $(@D)/zlib/include/",
    visibility = ["//visibility:private"],
)

cc_library(
    name = "minizip",
    srcs = [
        "contrib/minizip/ioapi.c",
        "contrib/minizip/unzip.c",
        "contrib/minizip/zip.c",
    ],
    hdrs = [
        "contrib/minizip/crypt.h",
        "contrib/minizip/ioapi.h",
        "contrib/minizip/unzip.h",
        "contrib/minizip/zip.h",
    ],
    copts = [
        "-Wno-deprecated-non-prototype",
        "-Wno-unused-but-set-variable",
    ],
    include_prefix = "third_party/zlib/minizip",
    strip_include_prefix = "contrib/minizip",
    deps = [":zlib"],
)

cc_library(
    name = "zlib",
    srcs = [
        "adler32.c",
        "compress.c",
        "crc32.c",
        "deflate.c",
        "gzclose.c",
        "gzlib.c",
        "gzread.c",
        "gzwrite.c",
        "infback.c",
        "inffast.c",
        "inflate.c",
        "inftrees.c",
        "trees.c",
        "uncompr.c",
        "zutil.c",
        # Include the un-prefixed headers in srcs to work
        # around the fact that zlib isn't consistent in its
        # choice of <> or "" delimiter when including itself.
    ] + _ZLIB_HEADERS,
    hdrs = _ZLIB_PREFIXED_HEADERS,
    copts = [
        "-Wno-deprecated-non-prototype",
        "-Wno-implicit-function-declaration",
        "-Wno-unused-variable",
    ],
    includes = ["zlib/include"],
    strip_include_prefix = "zlib/include",
    include_prefix = "third_party/zlib",
)
