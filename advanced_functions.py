# Advanced function features test

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

def higher_order_test():
    print("Testing higher-order functions...")
    
    # Test applying a function twice
    result = apply_twice(square, 3)
    print("square(square(3)) =", result)
    
    return result

# Test nested function calls and recursion
print("=== Advanced Function Tests ===")

# Fibonacci sequence
print("Fibonacci numbers:")
i = 0
while i < 8:
    fib = fibonacci(i)
    print("fib(", i, ") =", fib)
    i = i + 1

print()

# Higher-order functions
ho_result = higher_order_test()
print("Higher-order result:", ho_result)

print()

# Function composition
def compose_functions(x):
    return square(square(x))

comp_result = compose_functions(2)
print("Composed function result:", comp_result)

print("=== All tests completed ===")
