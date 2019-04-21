sh_test(
    name = "tracer_test",
    srcs = [
        "invoke_test.sh",
    ],
    data = [
        "//3rd_party:cpython_files",
        "//:bridge_tracer.so",
        "@io_opentracing_cpp//mocktracer:libmocktracer_plugin.so",
        "tracer_test.py",
    ],
)
