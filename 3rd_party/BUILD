load("@rules_foreign_cc//tools/build_defs:configure.bzl", "configure_make")

configure_make(
    name = "cpython",
    configure_options = [
        # Hack to work around https://github.com/bazelbuild/rules_foreign_cc/issues/239
        "CFLAGS='-Dredacted=\"redacted\"'"
    ],
    lib_source = "@com_github_python_cpython//:all",
    binaries = [
        "python",
    ],
)

filegroup(
    name = "cpython_files",
    srcs = [
        ":cpython",
    ],
    visibility = [
        "//visibility:public",
    ],
)
