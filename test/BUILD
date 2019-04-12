# py_test(
#     name = "t",
#     srcs = [
#         "t.py",
#     ],
# )

sh_test(
    name = "example_test",
    srcs = [
        "example_test.sh",
    ],
    data = [
        "//3rd_party:cpython_files",
        "//:bridge_tracer.so",
        "t.py",
    ],
)
