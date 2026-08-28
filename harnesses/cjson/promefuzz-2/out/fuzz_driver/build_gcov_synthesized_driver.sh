#! /bin/bash
cd /promefuzz
clang /promefuzz/database/cjson/latest/out/fuzz_driver/synthesized/*.c -o /promefuzz/database/cjson/latest/out/fuzz_driver/gcov_synthesized_driver --coverage -fsanitize=fuzzer -g -I/promefuzz/database/cjson/latest/code database/cjson/latest/bin_gcov/lib/libcjson.a