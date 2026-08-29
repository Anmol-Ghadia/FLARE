#! /bin/bash
cd /promefuzz
clang /promefuzz/database/liblhasa/latest/out/fuzz_driver/synthesized/*.c -o /promefuzz/database/liblhasa/latest/out/fuzz_driver/gcov_synthesized_driver --coverage -fsanitize=fuzzer -g -I/promefuzz/database/liblhasa/latest/code/lib/public database/liblhasa/latest/bin_gcov/lib/liblhasa.a