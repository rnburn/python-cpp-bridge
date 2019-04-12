cc_binary(
    name = "bridge_tracer.so",
    copts = [
        "-fPIC",
        "-DPy_LIMITED_API",
    ],
    linkopts = [
        "-shared",
        "-Wl,--unresolved-symbols=ignore-in-object-files",
    ],
    srcs = glob([
        # "src/tracer.h",
        # "src/tracer.cpp",
        # "src/module.cpp",
        "src/**/*.cpp", 
        "src/**/*.h"
    ]),
    deps = [
      "//3rd_party:cpython",
      "@io_opentracing_cpp//:opentracing",
    ],
    visibility = [
        "//visibility:public",
    ],
)
