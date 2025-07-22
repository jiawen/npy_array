# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Copied from
# https://github.com/llvm/llvm-project/blob/e8af89ef3de45b3058041388267d728c4ade16e8/utils/bazel/third_party_build/zlib-ng.BUILD#L1
# some modification applied (also licensed under the same license as the original work)

genrule(
    # The input template is identical to the CMake output.
    name = "zconf_gen",
    srcs = ["zconf.h.in"],
    outs = ["zconf.h"],
    cmd = "cp $(SRCS) $(OUTS)",
)

# Generates zlib.h (zlib compatibility API) from zlib.h.in.
genrule(
    # The input template is identical to the CMake output.
    name = "zlib_gen",
    srcs = ["zlib.h.in"],
    outs = ["zlib.h"],
    cmd = "cp $(SRCS) $(OUTS)",
)

# Generates zlib.h (zlib compatibility API) from zlib.h.in.
genrule(
    # The input template is identical to the CMake output.
    name = "zlib_name_mangling_gen",
    srcs = ["zlib_name_mangling.h.empty"],
    outs = ["zlib_name_mangling.h"],
    cmd = "cp $(SRCS) $(OUTS)",
)

cc_library(
    name = "zlib-ng",
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
        "zendian.h",
        "zmemory.h",
        "zutil.c",
        "zutil.h",
        "zutil_p.h",
        ":zconf_gen",
        ":zlib_name_mangling_gen",
    ],
    hdrs = [
        ":zlib_gen",
    ],
    copts = [
        "-DZLIB_COMPAT",
        "-DWITH_GZFILEOP",
        "-DWITH_OPTIM",
        "-DWITH_NEW_STRATEGIES",
        # For local builds you might want to add "-DWITH_NATIVE_INSTRUCTIONS"
        # here to improve performance. Native instructions aren't enabled in
        # the default config for reproducibility.
    ],
    # Clang includes zlib with angled instead of quoted includes, so we need
    # strip_include_prefix here.
    strip_include_prefix = ".",
    visibility = ["//visibility:public"],
)
