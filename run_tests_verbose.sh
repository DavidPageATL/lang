#!/bin/bash

# Shell script to run all test .py programs with detailed output
# Usage: ./run_tests_verbose.sh

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Counters
total_tests=0
passed_tests=0
failed_tests=0

# Build the project first
echo -e "${BLUE}Building the project...${NC}"
if [ ! -d "build" ]; then
    mkdir build
fi

cd build
if ! cmake .. > /dev/null 2>&1; then
    echo -e "${RED}❌ CMake configuration failed${NC}"
    exit 1
fi

if ! make > /dev/null 2>&1; then
    echo -e "${RED}❌ Build failed${NC}"
    exit 1
fi

cd ..
echo -e "${GREEN}✅ Build successful${NC}"
echo

# Check if the executable exists
if [ ! -f "build/LangProject" ]; then
    echo -e "${RED}❌ Executable not found: build/LangProject${NC}"
    exit 1
fi

# Array to store failed tests and their outputs
declare -A failed_test_outputs

echo -e "${BLUE}Running all test files with detailed output...${NC}"
echo "=================================================="

# Run each test file
for test_file in tests/*.py; do
    if [ -f "$test_file" ]; then
        filename=$(basename "$test_file")
        echo -e "${CYAN}Testing $filename...${NC}"
        
        # Run the test and capture output and exit code
        output=$(./build/LangProject "$test_file" 2>&1)
        exit_code=$?
        
        if [ $exit_code -eq 0 ]; then
            echo -e "${GREEN}✅ PASS${NC}"
            # Optionally show successful output (uncomment next line if desired)
            # echo "$output" | head -10
            ((passed_tests++))
        else
            echo -e "${RED}❌ FAIL (exit code: $exit_code)${NC}"
            echo "Output:"
            echo "$output" | sed 's/^/  /'
            echo
            ((failed_tests++))
            failed_test_outputs["$filename"]="$output"
        fi
        
        ((total_tests++))
        echo "---"
    fi
done

echo "=================================================="
echo

# Summary
echo -e "${BLUE}Test Summary:${NC}"
echo "Total tests: $total_tests"
echo -e "Passed: ${GREEN}$passed_tests${NC}"
echo -e "Failed: ${RED}$failed_tests${NC}"

if [ $failed_tests -gt 0 ]; then
    echo
    echo -e "${YELLOW}Failed tests summary:${NC}"
    for test_name in "${!failed_test_outputs[@]}"; do
        echo "  - $test_name"
    done
    echo
    echo -e "${YELLOW}To debug a specific test, run:${NC}"
    echo "  ./build/LangProject tests/<test_file>"
    exit 1
else
    echo
    echo -e "${GREEN}🎉 All tests passed!${NC}"
    exit 0
fi
