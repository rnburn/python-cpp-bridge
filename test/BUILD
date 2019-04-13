sh_test(
    name = "example_test",
    srcs = [
        "example_test.sh",
    ],
    data = [
        "//3rd_party:cpython_files",
        "//:bridge_tracer.so",
        "@io_opentracing_cpp//mocktracer:libmocktracer_plugin.so",
        "t.py",
        "mocktracer-config.json",
    ],
)
