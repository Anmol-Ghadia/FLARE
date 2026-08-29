#! /bin/bash

cd /promefuzz
clang /promefuzz/database/liblhasa/latest/out/fuzz_driver/synthesized/*.c -o /promefuzz/database/liblhasa/latest/out/fuzz_driver/synthesized_driver -fsanitize=fuzzer,address,undefined -g -I/promefuzz/database/liblhasa/latest/code/lib/public database/liblhasa/latest/bin_asan/lib/liblhasa.a
