#! /bin/bash
cd /promefuzz
clang++ /promefuzz/database/libpng/latest/out/fuzz_driver/synthesized/*.cpp -o /promefuzz/database/libpng/latest/out/fuzz_driver/cov_synthesized_driver -fprofile-instr-generate -fcoverage-mapping -fsanitize=fuzzer -g -I/promefuzz/database/libpng/latest/code database/libpng/latest/bin_cov/lib/libpng16.a -Idatabase/libpng/latest/bin_cov/include -lz