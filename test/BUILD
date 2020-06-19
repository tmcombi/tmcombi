cc_test(
    name = "hello-test",
    size = "small",
    srcs = ["hello-test.cc"],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:gtest_main",
        "//main:hello-greet",
    ],
)

cc_binary(
    name = "boost-test",
    srcs = ["boost-test.cc"],
)
