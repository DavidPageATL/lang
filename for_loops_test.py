# Test for loops

print("=== For Loop Tests ===")

# Test iterating over a list
print("Iterating over numbers:")
numbers = [1, 2, 3, 4, 5]
for num in numbers:
    print("Number:", num)

print()

# Test iterating over a list with mixed types
print("Iterating over mixed list:")
mixed = [1, "hello", True, None]
for item in mixed:
    print("Item:", item)

print()

# Test iterating over dictionary keys
print("Iterating over dictionary keys:")
person = {"name": "Alice", "age": 30, "city": "New York"}
for key in person:
    print("Key:", key, "Value:", person[key])

print()

# Test nested loops
print("Nested loops:")
matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
for row in matrix:
    print("Row:", row)
    for item in row:
        print("  Item:", item)

print()

# Test for loop with empty collections
print("Empty collections:")
empty_list = []
empty_dict = {}

print("Empty list iteration:")
for item in empty_list:
    print("This should not print")
print("Done with empty list")

print("Empty dict iteration:")
for key in empty_dict:
    print("This should not print")
print("Done with empty dict")

print()

# Test for loop with computation
print("Computing squares:")
for i in [1, 2, 3, 4, 5]:
    square = i * i
    print("Square of", i, "is", square)

print()
print("=== All for loop tests completed ===")
