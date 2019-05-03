load(
    "//bazel:python_bridge_build_system.bzl",
    "python_bridge_package",
)

load("@rules_foreign_cc//tools/build_defs:configure.bzl", "configure_make")

python_bridge_package()

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

py_runtime(
    name = "python-3.6.0",
    interpreter_path = "/usr/bin/python3",
)
