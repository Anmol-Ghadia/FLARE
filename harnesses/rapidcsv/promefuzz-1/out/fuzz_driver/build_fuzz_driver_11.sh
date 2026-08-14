#! /bin/bash

cd /promefuzz
clang++ /promefuzz/database/rapidcsv/latest/out/fuzz_driver/fuzz_driver_11.cpp -o /promefuzz/database/rapidcsv/latest/out/fuzz_driver/fuzz_driver_11 -fsanitize=fuzzer,address,undefined -g -I/promefuzz/database/rapidcsv/latest/code/src 
