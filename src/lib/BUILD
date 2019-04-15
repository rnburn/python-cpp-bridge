load(
    "//bazel:python_bridge_build_system.bzl",
    "python_bridge_cc_library",
    "python_bridge_package",
)

python_bridge_package()

python_bridge_cc_library(
    name = "bridge_tracer_lib",
    srcs = glob([
      "*.h",
      "*.cpp",
    ]),
    deps = [
        "//include/python_bridge_tracer:module_interface",
    ],
    external_deps = [
        "@io_opentracing_cpp//:opentracing",
        "//3rd_party:cpython",
    ],
)

