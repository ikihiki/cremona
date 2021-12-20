load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Hedron's Compile Commands Extractor for Bazel
# https://github.com/hedronvision/bazel-compile-commands-extractor
http_archive(
    name = "hedron_compile_commands",

    # Replace the commit hash in both places (below) with the latest. 
    # Even better, set up Renovate and let it do the work for you (see "Suggestion: Updates" below).
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/486d6eaf87255b7e15640bb9add58f0a77c1e2b6.tar.gz",
    strip_prefix = "bazel-compile-commands-extractor-486d6eaf87255b7e15640bb9add58f0a77c1e2b6",
)
load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")
hedron_compile_commands_setup()

http_archive(
    name = "rules_foreign_cc",
    # TODO: Get the latest sha256 value from a bazel debug message or the latest 
    #       release on the releases page: https://github.com/bazelbuild/rules_foreign_cc/releases
    #
    # sha256 = "...",
    strip_prefix = "rules_foreign_cc-f76d9281bd7ae1f36179740ba20db3d58cd3b7a3",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/f76d9281bd7ae1f36179740ba20db3d58cd3b7a3.tar.gz",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

#Google Test
http_archive(
  name = "com_google_googletest",
  urls = ["https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip"],
  strip_prefix = "googletest-609281088cfefc76f9d0ce82e1ff6c30cc3591e5",
)

http_archive(
  name = "com_ludocode_mpack",
  urls = ["https://github.com/ludocode/mpack/releases/download/v1.1/mpack-amalgamation-1.1.tar.gz"],
  strip_prefix = "mpack-amalgamation-1.1",
  build_file_content = """
filegroup(
    name = "mpack",
    srcs = [
        "src/mpack/mpack.c",
        "src/mpack/mpack.h",
    ],
    visibility = ["//visibility:public"],
)
"""
)
