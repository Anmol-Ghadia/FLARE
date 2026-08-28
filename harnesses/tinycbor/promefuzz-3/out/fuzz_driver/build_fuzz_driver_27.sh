#! /bin/bash

cd /promefuzz
clang /promefuzz/database/tinycbor/latest/out/fuzz_driver/fuzz_driver_27.c -o /promefuzz/database/tinycbor/latest/out/fuzz_driver/fuzz_driver_27 -fsanitize=fuzzer,address,undefined -g -I/promefuzz/database/tinycbor/latest/code/src database/tinycbor/latest/bin_asan/lib/libtinycbor.a -Idatabase/tinycbor/latest/bin_asan/include/tinycbor -Idatabase/tinycbor/latest/code -Idatabase/tinycbor/latest/code/src
