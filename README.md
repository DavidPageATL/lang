# Python-like Language Parser and Interpreter

A C++ implementation of a lexer, parser, and interpreter for a Python-like programming language.

## Features

### Language Support
- **Variables and Assignment**: `x = 10`, `name = "Alice"`
- **Data Types**: Numbers (int/float), strings, booleans (`True`/`False`), `None`
- **Collections**: Lists (`[1, 2, 3]`) and dictionaries (`{"key": "value"}`)
- **Indexing**: List and dictionary access (`list[0]`, `dict["key"]`)
- **Arithmetic Operations**: `+`, `-`, `*`, `/`, `%`, `**` (power)
- **Comparison Operations**: `==`, `!=`, `<`, `<=`, `>`, `>=`
- **Logical Operations**: `and`, `or`, `not`
- **Control Flow**: `if`/`elif`/`else` statements, `while` loops, `for` loops (`for item in iterable`)
- **Function Definitions**: `def` statements with parameters, return values, and closures
- **Function Calls**: Built-in `print()` function and user-defined functions
- **Comments**: `# This is a comment`
- **Indentation-based Blocks**: Python-style indentation

### Components

1. **Lexer** (`src/lexer.h/cpp`)
   - Tokenizes source code into tokens
   - Handles Python-style indentation with INDENT/DEDENT tokens
   - Supports string literals, numbers, identifiers, and operators
   - Comment parsing and whitespace handling

2. **Parser** (`src/parser.h/cpp`)
   - Recursive descent parser
   - Builds an Abstract Syntax Tree (AST)
   - Operator precedence handling
   - Expression and statement parsing

3. **Interpreter** (`src/interpreter.h/cpp`)
   - Tree-walking interpreter
   - Variable environment with scoping
   - Built-in function support
   - Runtime type checking

## Building the Project

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Generate the build files:
```bash
cmake ..
```

3. Build the project:
```bash
make
```

4. Run the executable:
```bash
./LangProject [filename]
```

## Usage

### Interactive Mode
Run without arguments to see a demo program:
```bash
./LangProject
```

### File Mode
Run with a Python-like source file:
```bash
./LangProject example.py
```

### Build and Run Script
Use the convenience script:
```bash
./build_and_run.sh
```

## Example Programs

### Basic Example (`example.py`)
```python
# Variable assignments
name = "Alice"
age = 25
height = 5.6
is_student = True

# Print statements
print("Personal Information:")
print("Name:", name)
print("Age:", age)

# Arithmetic operations
birth_year = 2024 - age
print("Birth year:", birth_year)

# Conditional statements
if age >= 18:
    print("You are an adult")
    if age >= 65:
        print("You are a senior citizen")
    else:
        print("You are a working-age adult")
else:
    print("You are a minor")

# Loop example
print("\nCounting to 5:")
count = 1
while count <= 5:
    print("Count:", count)
    count = count + 1

print("\nProgram completed!")
```

### Lists and Dictionaries Example (`lists_dicts_test.py`)
```python
# Lists
numbers = [1, 2, 3, 4, 5]
print("Numbers:", numbers)
print("First:", numbers[0])
print("Last:", numbers[-1])

# Dictionaries
person = {"name": "Alice", "age": 30, "city": "New York"}
print("Person:", person)
print("Name:", person["name"])

# Mixed and nested structures
mixed = [1, "hello", True, None]
nested = [{"name": "Bob", "scores": [95, 87, 92]}]
print("Mixed:", mixed)
print("Nested:", nested)
```

### Function Definition Example (`advanced_functions.py`)
```python
def fibonacci(n):
    if n <= 1:
        return n
    else:
        return fibonacci(n - 1) + fibonacci(n - 2)

def square(x):
    return x * x

def apply_twice(func, value):
    result = func(value)
    return func(result)

# Usage
print("Fibonacci(7):", fibonacci(7))
print("Apply twice square(3):", apply_twice(square, 3))
```

### For Loop Example (`for_loops_test.py`)
```python
# Iterate over lists
numbers = [1, 2, 3, 4, 5]
for num in numbers:
    print("Number:", num)

# Iterate over dictionary keys
person = {"name": "Alice", "age": 30}
for key in person:
    print("Key:", key, "Value:", person[key])

# Nested loops with computations
for i in [1, 2, 3, 4, 5]:
    square = i * i
    print("Square of", i, "is", square)
```

### Language Grammar (Simplified)

```
program        → statement*
statement      → assignment | if_stmt | while_stmt | expr_stmt
assignment     → IDENTIFIER "=" expression
if_stmt        → "if" expression ":" block ("else" ":" block)?
while_stmt     → "while" expression ":" block
block          → INDENT statement+ DEDENT
expression     → logical_or
logical_or     → logical_and ("or" logical_and)*
logical_and    → equality ("and" equality)*
equality       → comparison (("==" | "!=") comparison)*
comparison     → term ((">" | ">=" | "<" | "<=") term)*
term           → factor (("+" | "-") factor)*
factor         → power (("*" | "/" | "%") power)*
power          → unary ("**" unary)*
unary          → ("not" | "-") unary | call
call           → primary ("(" arguments? ")")*
primary        → NUMBER | STRING | "True" | "False" | IDENTIFIER | "(" expression ")"
```

## Project Structure

```
lang/
├── CMakeLists.txt          # CMake build configuration
├── README.md               # This file
├── .gitignore             # Git ignore file
├── build_and_run.sh       # Build and run script
├── example.py             # Example Python-like program
└── src/                   # Source code directory
    ├── main.cpp           # Main entry point
    ├── lexer.h/cpp        # Lexical analyzer
    ├── parser.h/cpp       # Syntax analyzer
    └── interpreter.h/cpp  # Runtime interpreter
```

## Requirements

- CMake 3.10 or higher
- C++17 compatible compiler (GCC, Clang, etc.)
- Make build system

## Development

### Adding New Features

1. **New Operators**: Add to `TokenType` enum and lexer recognition
2. **New Statements**: Add AST nodes in `parser.h` and parsing logic
3. **New Built-ins**: Add to `setupBuiltins()` in interpreter
4. **New Data Types**: Extend the `Value` variant type

### Testing

The interpreter provides detailed output showing:
- Tokenization results (all tokens with types and values)
- Parse results (AST structure information)
- Execution results (program output)

This makes it easy to debug and understand how your Python-like code is being processed.

## Future Enhancements

- ~~Function definitions (`def` statements)~~ ✅ **Completed**
- ~~Lists and dictionaries~~ ✅ **Completed**
- ~~For loops (`for item in iterable`)~~ ✅ **Completed**
- Classes and objects
- Import system
- Error handling (`try`/`except`)
- More built-in functions (len, range, etc.)
