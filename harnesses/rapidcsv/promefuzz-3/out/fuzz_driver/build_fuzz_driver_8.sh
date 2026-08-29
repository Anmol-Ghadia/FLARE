#! /bin/bash

cd /promefuzz
clang++ /promefuzz/database/rapidcsv/latest/out/fuzz_driver/fuzz_driver_8.cpp -o /promefuzz/database/rapidcsv/latest/out/fuzz_driver/fuzz_driver_8 -fsanitize=fuzzer,address,undefined -g -I/promefuzz/database/rapidcsv/latest/code/src 
