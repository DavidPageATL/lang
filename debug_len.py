# Simple test to debug function calls
print("Testing direct calls...")
print("len works:", len([1, 2, 3]))

# Test what len evaluates to
print("len evaluates to:", len)

# Test the specific error case
print("Testing len(42)...")
len(42)
