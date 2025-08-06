# Test Scripts Documentation

This directory contains several shell scripts for running the Python-like language test programs:

## Scripts

### 1. `run_tests.sh`
**Comprehensive test runner** - Runs all .py files in the tests/ directory
- Automatically builds the project if needed
- Shows pass/fail status for each test
- Provides summary with counts
- Lists failed tests for debugging
- Colored output for better readability

Usage:
```bash
./run_tests.sh
```

### 2. `run_tests_verbose.sh`
**Detailed test runner** - Like run_tests.sh but shows full output from failed tests
- Same features as run_tests.sh
- Shows complete error output for debugging
- Useful for troubleshooting specific test failures

Usage:
```bash
./run_tests_verbose.sh
```

### 3. `run_basic_tests.sh`
**Basic test runner** - Runs only a curated set of fundamental tests
- Faster execution (only 8 core tests)
- Good for quick validation during development
- Tests: super_simple.py, simple_test.py, print_test.py, simple_if_test.py, test_builtins.py, lists_dicts_test.py, for_loops_test.py, simple_try_test.py

Usage:
```bash
./run_basic_tests.sh
```

## Test Categories

The tests/ directory contains 43 test files covering:
- **Basic functionality**: print, variables, arithmetic
- **Control flow**: if/else, while loops, for loops
- **Data structures**: lists, dictionaries
- **Functions**: definition, calls, parameters
- **Classes**: definition, instantiation, methods
- **Exception handling**: try/except, raise
- **Modules**: import, from-import
- **Built-in functions**: len, range, str, int, float, type, bool, min, max, sum

## Running Individual Tests

To run a specific test file:
```bash
./build/LangProject tests/<test_file.py>
```

## Current Status

✅ All 43 tests passing (as of last run)
✅ No compiler warnings
✅ Complete language feature coverage
