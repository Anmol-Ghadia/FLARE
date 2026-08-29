#! /bin/bash
cd /promefuzz
clang++ /promefuzz/database/rapidcsv/latest/out/fuzz_driver/synthesized/*.cpp -o /promefuzz/database/rapidcsv/latest/out/fuzz_driver/cov_synthesized_driver -fprofile-instr-generate -fcoverage-mapping -fsanitize=fuzzer -g -I/promefuzz/database/rapidcsv/latest/code/src 