#! /bin/bash

cd /promefuzz
clang /promefuzz/database/cjson/latest/out/fuzz_driver/fuzz_driver_15.c -o /promefuzz/database/cjson/latest/out/fuzz_driver/fuzz_driver_15 -fsanitize=fuzzer,address,undefined -g -I/promefuzz/database/cjson/latest/code database/cjson/latest/bin_asan/lib/libcjson.a
