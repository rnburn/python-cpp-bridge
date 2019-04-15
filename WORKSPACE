workspace(name = "opentracing_python_cpp_bridge")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

# load("//bazel:cc_configure.bzl", "cc_configure")
# cc_configure()

all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""

git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    commit = "3721d32c14d3639ff94320c780a60a6e658fb033",
)

http_archive(
   name = "rules_foreign_cc",
   strip_prefix = "rules_foreign_cc-master",
   url = "https://github.com/bazelbuild/rules_foreign_cc/archive/master.zip",
)

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies([
])

git_repository(
    name = "io_opentracing_cpp",
    remote = "https://github.com/opentracing/opentracing-cpp",
    commit = "ac50154a7713877f877981c33c3375003b6ebfe1",
)

http_archive(
    name = "com_github_openssl_openssl",
    build_file_content = all_content,
    strip_prefix = "openssl-OpenSSL_1_1_1",
    urls = [
        "https://github.com/openssl/openssl/archive/OpenSSL_1_1_1.tar.gz",
    ],
)

http_archive(
    name = "com_github_python_cpython",
    build_file_content = all_content,
    strip_prefix = "cpython-3.7.3",
    urls = [
        "https://github.com/python/cpython/archive/v3.7.3.tar.gz",
    ],
)

git_repository(
    name = "io_bazel_rules_python",
    remote = "https://github.com/bazelbuild/rules_python.git",
    commit = "965d4b4a63e6462204ae671d7c3f02b25da37941",
)


# Only needed for PIP support:
load("@io_bazel_rules_python//python:pip.bzl", "pip_repositories")

pip_repositories()
