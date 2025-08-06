# Test try/except error handling

print("=== Testing Basic Try/Except ===")

try:
    print("In try block")
    raise("Something went wrong!")
    print("This should not print")
except:
    print("Caught exception!")

print("\n=== Testing Specific Exception Types ===")

try:
    print("About to raise ValueError")
    raise("ValueError", "Invalid value provided")
except ValueError as e:
    print("Caught ValueError:", e)

print("\n=== Testing Multiple Except Clauses ===")

try:
    print("Raising TypeError")
    raise("TypeError", "Type mismatch")
except ValueError:
    print("This won't execute")
except TypeError as error:
    print("Caught TypeError:", error)
except:
    print("This is a catch-all")

print("\n=== Testing Runtime Error Handling ===")

try:
    # This should cause a runtime error (undefined variable)
    x = undefined_variable
except RuntimeError as e:
    print("Caught runtime error:", e)

print("\n=== Testing Division by Zero ===")

try:
    result = 10 / 0
except:
    print("Caught division by zero!")

print("\n=== Testing Nested Try/Except ===")

try:
    print("Outer try")
    try:
        print("Inner try")
        raise("InnerError", "From inner block")
    except InnerError:
        print("Caught in inner except")
        raise("OuterError", "Re-raising from inner")
except OuterError as e:
    print("Caught in outer except:", e)

print("\nAll exception tests completed!")
