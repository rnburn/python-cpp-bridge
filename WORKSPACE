workspace(name = "apache_bazel_example")

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

http_archive(
    name = "com_github_python_cpython",
    build_file_content = all_content,
    strip_prefix = "cpython-2.7.16",
    urls = [
        "https://github.com/python/cpython/archive/v2.7.16.tar.gz",
    ],
)

git_repository(
    name = "io_bazel_rules_python",
    remote = "https://github.com/bazelbuild/rules_python.git",
    commit = "965d4b4a63e6462204ae671d7c3f02b25da37941",
)

http_archive(
    name = "com_github_pybind_pybind11",
    strip_prefix = "pybind11-2.2.4",
    urls = [
        "https://github.com/pybind/pybind11/archive/v2.2.4.tar.gz",
    ],
    sha256 = "b69e83658513215b8d1443544d0549b7d231b9f201f6fc787a2b2218b408181e",
    build_file = "//build:pybind11.BUILD",
)


# Only needed for PIP support:
load("@io_bazel_rules_python//python:pip.bzl", "pip_repositories")

pip_repositories()
