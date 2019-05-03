load("@python_pip_deps//:requirements.bzl", "requirement")

load(
    "//bazel:python_bridge_build_system.bzl",
    "python_bridge_test",
    "python_bridge_package",
)

python_bridge_package()

python_bridge_test(
    name = "tracer_test",
    srcs = [
        "tracer_test.py",
    ],
    data = [
        "//:bridge_tracer.so",
        "@io_opentracing_cpp//mocktracer:libmocktracer_plugin.so",
    ],
    deps = [
        requirement("opentracing"),
    ],
)
