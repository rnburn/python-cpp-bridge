load(
    "//bazel:python_bridge_build_system.bzl",
    "python_bridge_cc_library",
    "python_bridge_package",
)

python_bridge_package()

python_bridge_cc_library(
    name = "module_interface",
    hdrs = [
        "module.h",
    ],
    external_deps = [
        "@io_opentracing_cpp//:opentracing",
        "//3rd_party:cpython",
    ],
)
