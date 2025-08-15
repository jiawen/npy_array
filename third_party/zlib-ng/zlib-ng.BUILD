# Bazel BUILD file to be "patched" into zlib-ng via http_archive.
#
# This file depends on //third_party/zlib-ng:BUILD.bazel, which is a companion
# BUILD file that supplies the configure script.

# Generates configuration headers from templates.
genrule(
    name = "configure",
    srcs = [
        "zconf.h.in",
        "zlib.h.in",
    ],
    outs = [
        "zconf.h",
        "zlib.h",
    ],
    cmd = "$(location @//third_party/zlib-ng:gen_headers) $(@D) $(SRCS)",
    tools = ["@//third_party/zlib-ng:gen_headers"],
)

# Copies the "empty" (no name mangling) template to use as the header.
genrule(
    name = "copy_zlib_name_mangling_empty",
    srcs = ["zlib_name_mangling.h.empty"],
    outs = ["zlib_name_mangling.h"],
    cmd = "cp $(SRCS) $(OUTS)",
)

ARCH_GENERIC_SRCS = [
    "arch/generic/adler32_c.c",
    "arch/generic/adler32_fold_c.c",
    "arch/generic/chunk_permute_table.h",
    "arch/generic/chunkset_c.c",
    "arch/generic/compare256_c.c",
    "arch/generic/compare256_p.h",
    "arch/generic/crc32_braid_c.c",
    "arch/generic/crc32_fold_c.c",
    "arch/generic/generic_functions.h",
    "arch/generic/slide_hash_c.c",
]

ARM_SRCS = [
    "acle_intrins.h",
    "adler32_neon.c",
    "arm_features.c",
    "arm_features.h",
    "arm_functions.h",
    "chunkset_neon.c",
    "compare256_neon.c",
    "crc32_acle.c",
    "Makefile.in",
    "neon_intrins.h",
    "slide_hash_armv6.c",
    "slide_hash_neon.c",
]

# Builds zlib with the zlib compatible API.
#
# TODO(jiawen): Add "zlib-ng" for the zlib-ng API.
#
# TODO(jiawen): Add "-DWITH_NATIVE_INSTRUCTIONS" here to improve performance.
cc_library(
    name = "zlib",
    srcs = [
        "adler32.c",
        "adler32_p.h",
        "arch_functions.h",
        "chunkset_tpl.h",
        "compare256_rle.h",
        "compress.c",
        "cpu_features.c",
        "cpu_features.h",
        "crc32.c",
        "crc32.h",
        "crc32_braid_comb.c",
        "crc32_braid_comb_p.h",
        "crc32_braid_p.h",
        "crc32_braid_tbl.h",
        "deflate.c",
        "deflate.h",
        "deflate_fast.c",
        "deflate_huff.c",
        "deflate_medium.c",
        "deflate_p.h",
        "deflate_quick.c",
        "deflate_rle.c",
        "deflate_slow.c",
        "deflate_stored.c",
        "fallback_builtins.h",
        "functable.c",
        "functable.h",
        "gzguts.h",
        "gzlib.c",
        "gzwrite.c",
        "infback.c",
        "inffast_tpl.h",
        "inffixed_tbl.h",
        "inflate.c",
        "inflate.h",
        "inflate_p.h",
        "inftrees.c",
        "inftrees.h",
        "insert_string.c",
        "insert_string_roll.c",
        "insert_string_tpl.h",
        "match_tpl.h",
        "trees.c",
        "trees.h",
        "trees_emit.h",
        "trees_tbl.h",
        "uncompr.c",
        "zbuild.h",
        "zconf.h",
        "zendian.h",
        "zlib_name_mangling.h",
        "zmemory.h",
        "zutil.c",
        "zutil.h",
        "zutil_p.h",
    ] + ARCH_GENERIC_SRCS,
    hdrs = ["zlib.h"],
    local_defines = [
        "ZLIB_COMPAT=1",
        "WITH_GZFILEOP=1",
        "WITH_OPTIM=1",
        "WITH_NEW_STRATEGIES=1",
        "HAVE_POSIX_MEMALIGN=1",
        "HAVE_ALIGNED_ALLOC=1",
        "HAVE_VISIBILITY_HIDDEN=1",
        "HAVE_VISIBILITY_INTERNAL=1",
        "HAVE_ATTRIBUTE_ALIGNED=1",
        "HAVE_BUILTIN_ASSUME_ALIGNED=1",
        "HAVE_BUILTIN_CTZ=1",
        "HAVE_BUILTIN_CTZLL=1",
        "HAVE_PTRDIFF_T=1",
        "HAVE_FSEEKO=1",
        "HAVE_STRERROR=1",
        "_LARGEFILE64_SOURCE=1",
        "__USE_LARGEFILE64=1",
    ],
    # Clang includes zlib with angled instead of quoted includes, so we need
    # strip_include_prefix here.
    strip_include_prefix = ".",
    visibility = ["//visibility:public"],
)
