#! /bin/bash
cd /promefuzz
clang++ /promefuzz/database/rapidcsv/latest/out/fuzz_driver/synthesized/*.cpp -o /promefuzz/database/rapidcsv/latest/out/fuzz_driver/gcov_synthesized_driver --coverage -fsanitize=fuzzer -g -I/promefuzz/database/rapidcsv/latest/code/src 