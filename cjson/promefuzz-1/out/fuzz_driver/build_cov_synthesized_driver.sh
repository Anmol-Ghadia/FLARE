#! /bin/bash
cd /promefuzz
clang /promefuzz/database/cjson/latest/out/fuzz_driver/synthesized/*.c -o /promefuzz/database/cjson/latest/out/fuzz_driver/cov_synthesized_driver -fprofile-instr-generate -fcoverage-mapping -fsanitize=fuzzer -g -I/promefuzz/database/cjson/latest/code database/cjson/latest/bin_cov/lib/libcjson.a