# Test if len is being found correctly
print("Testing len discovery...")

# First check if len exists as a variable
print("len exists:", len)

# Test if it evaluates to a string
print("Type of len is string?", type(len) == "str")

# Test the actual call
print("Calling len with valid argument...")
result = len([1, 2])
print("Result:", result)
