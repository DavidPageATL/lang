# Test specific exception handling
print("=== Testing specific error handling ===")
try:
    result = len(42)
    print("Result:", result)
except TypeError as e:
    print("TypeError caught:", e)
except RuntimeError as e:
    print("RuntimeError caught:", e)
except:
    print("Some other error occurred")

print("Done with error test.")
