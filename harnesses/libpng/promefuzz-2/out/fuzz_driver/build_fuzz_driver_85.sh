#! /bin/bash

cd /promefuzz
clang++ /promefuzz/database/libpng/latest/out/fuzz_driver/fuzz_driver_85.cpp -o /promefuzz/database/libpng/latest/out/fuzz_driver/fuzz_driver_85 -fsanitize=fuzzer,address,undefined -g -I/promefuzz/database/libpng/latest/code database/libpng/latest/bin_asan/lib/libpng16.a -Idatabase/libpng/latest/bin_asan/include -lz
