load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_test")
load("@rules_foreign_cc//foreign_cc:defs.bzl", "make")

cc_binary(
    name = "hello-world",
    srcs =  glob(["src/*.c"]) + glob(["include/*.h"]),
)

cc_test(
  name = "hello-test",
  size = "small",
  srcs = glob(["src/*.c"], exclude = ["src/main.c"]) + glob(["include/*.h"]) + glob(["test/*.cc"]) + ["@com_ludocode_mpack//:mpack"],
  deps = ["@com_google_googletest//:gtest_main"],
  defines = ["MPACK_HAS_CONFIG=1"],
  copts = ["-iquote external/com_ludocode_mpack/src/mpack", "-iquote test/include"]
)



make(
  name = "hello-mod",
  lib_source = "//test/mod:srcs",
  targets = ["bazel-build"],
  out_bin_dir = "",
  out_binaries = ["main.ko"],
  deps = ["@com_ludocode_mpack//:mpack"]
)