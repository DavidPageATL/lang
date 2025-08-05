# Function definition example

def greet(name):
    print("Hello,", name, "!")
    return "Greeted " + name

def add(a, b):
    result = a + b
    print("Adding", a, "and", b, "equals", result)
    return result

def factorial(n):
    if n <= 1:
        return 1
    else:
        return n * factorial(n - 1)

# Test the functions
print("=== Function Tests ===")

# Simple function call
greeting_result = greet("Alice")
print("Greeting result:", greeting_result)

# Function with return value
sum_result = add(15, 25)
print("Sum result:", sum_result)

# Recursive function
fact_5 = factorial(5)
print("5! =", fact_5)

# Function as value
my_func = add
result = my_func(10, 20)
print("Function as value result:", result)

print("=== Done ===")
