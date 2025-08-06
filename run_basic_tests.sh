#!/bin/bash

# Simple shell script to run basic test programs
# Usage: ./run_basic_tests.sh

# Build if needed
if [ ! -f "build/LangProject" ]; then
    echo "Building project..."
    mkdir -p build
    cd build
    cmake .. && make
    cd ..
fi

# Basic tests to run (most likely to work)
basic_tests=(
    "super_simple.py"
    "simple_test.py"
    "print_test.py"
    "simple_if_test.py"
    "test_builtins.py"
    "lists_dicts_test.py"
    "for_loops_test.py"
    "simple_try_test.py"
)

echo "Running basic tests..."
echo "====================="

passed=0
total=0

for test in "${basic_tests[@]}"; do
    if [ -f "tests/$test" ]; then
        echo -n "Running $test... "
        if ./build/LangProject "tests/$test" > /dev/null 2>&1; then
            echo "PASS"
            ((passed++))
        else
            echo "FAIL"
        fi
        ((total++))
    else
        echo "Test file not found: tests/$test"
    fi
done

echo "====================="
echo "Results: $passed/$total tests passed"
