#! /bin/bash
cd /promefuzz
clang++ /promefuzz/database/libpng/latest/out/fuzz_driver/synthesized/*.cpp -o /promefuzz/database/libpng/latest/out/fuzz_driver/gcov_synthesized_driver --coverage -fsanitize=fuzzer -g -I/promefuzz/database/libpng/latest/code database/libpng/latest/bin_gcov/lib/libpng16.a -Idatabase/libpng/latest/bin_gcov/include -lz