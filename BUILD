cc_binary(
    name = "example.so",
    copts = [
        "-fPIC",
    ],
    linkopts = [
        "-shared",
        "-Wl,--unresolved-symbols=ignore-in-object-files",
    ],
    srcs = glob(["src/**/*.cpp", "src/**/*.h"]),
    deps = [
      "@com_github_pybind_pybind11//:pybind11",
      "//3rd_party:cpython",
    ],
    visibility = [
        "//visibility:public",
    ],
)
