#!/bin/bash

make CONFIG=coverage
cd coverage-x86_64/
./salsasm_test
cd ..
lcov -c --directory . -o coverage-x86_64/coverage.info
lcov -r coverage-x86_64/coverage.info "/home/ryan/src/salsasm/test/main.cpp" -o coverage-x86_64/coverage.info
lcov -r coverage-x86_64/coverage.info "*gtest*" -o coverage-x86_64/coverage.info
lcov -r coverage-x86_64/coverage.info "*bits*" -o coverage-x86_64/coverage.info
lcov -r coverage-x86_64/coverage.info "*ext*" -o coverage-x86_64/coverage.info
lcov -r coverage-x86_64/coverage.info "*iomanip*" -o coverage-x86_64/coverage.info
lcov -r coverage-x86_64/coverage.info "*iostream*" -o coverage-x86_64/coverage.info
lcov -r coverage-x86_64/coverage.info "*new*" -o coverage-x86_64/coverage.info
lcov -r coverage-x86_64/coverage.info "*sstream*" -o coverage-x86_64/coverage.info
lcov -r coverage-x86_64/coverage.info "*typeinfo*" -o coverage-x86_64/coverage.info
genhtml coverage-x86_64/coverage.info --output-directory coverage-x86_64/report
