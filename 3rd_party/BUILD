load("@rules_foreign_cc//tools/build_defs:configure.bzl", "configure_make")

configure_make(
    name = "openssl",
    configure_command = "config",
    configure_options = [
        "no-shared",
    ],
    lib_source = "@com_github_openssl_openssl//:all",
    static_libraries = [
        "libcrypto.a",
        "libssl.a",
    ],
)

configure_make(
    name = "cpython",
    configure_options = [
        "--with-openssl=$EXT_BUILD_DEPS/openssl",
        # Hack to work around https://github.com/bazelbuild/rules_foreign_cc/issues/239
        "CFLAGS='-Dredacted=\"redacted\"'",
    ],
    lib_source = "@com_github_python_cpython//:all",
    out_include_dir = "include/python3.7m",
    binaries = [
        "python3",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":openssl",
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
