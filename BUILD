cc_library(
    name = "concurrent_queue",
    hdrs = glob(["*.h"]),
)

cc_binary(
    name = "queue_test",
    srcs = ["queue_test.cc"],
    deps = ["concurrent_queue"],
)
