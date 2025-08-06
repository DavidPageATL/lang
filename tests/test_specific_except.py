# Test specific exception types
print("Testing ValueError")

try:
    print("About to raise")
    raise("ValueError", "Something is wrong")
except ValueError as e:
    print("Caught ValueError:", e)

print("Done")
