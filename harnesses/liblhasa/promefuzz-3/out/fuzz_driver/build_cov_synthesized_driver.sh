#! /bin/bash
cd /promefuzz
clang /promefuzz/database/liblhasa/latest/out/fuzz_driver/synthesized/*.c -o /promefuzz/database/liblhasa/latest/out/fuzz_driver/cov_synthesized_driver -fprofile-instr-generate -fcoverage-mapping -fsanitize=fuzzer -g -I/promefuzz/database/liblhasa/latest/code/lib/public database/liblhasa/latest/bin_cov/lib/liblhasa.a