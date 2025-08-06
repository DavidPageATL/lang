#!/bin/bash

# Shell script to run all test .py programs
# Usage: ./run_tests.sh

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
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

# Array to store failed tests
failed_test_files=()

echo -e "${BLUE}Running all test files...${NC}"
echo "=================================================="

# Run each test file
for test_file in tests/*.py; do
    if [ -f "$test_file" ]; then
        filename=$(basename "$test_file")
        echo -n "Testing $filename... "
        
        # Run the test and capture output and exit code
        if ./build/LangProject "$test_file" > /dev/null 2>&1; then
            echo -e "${GREEN}✅ PASS${NC}"
            ((passed_tests++))
        else
            echo -e "${RED}❌ FAIL${NC}"
            ((failed_tests++))
            failed_test_files+=("$filename")
        fi
        
        ((total_tests++))
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
    echo -e "${YELLOW}Failed tests:${NC}"
    for failed_test in "${failed_test_files[@]}"; do
        echo "  - $failed_test"
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
