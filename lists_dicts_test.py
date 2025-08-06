# Test lists and dictionaries

print("=== List Tests ===")

# Create a list
numbers = [1, 2, 3, 4, 5]
print("Numbers list:", numbers)

# Access elements
print("First element:", numbers[0])
print("Last element:", numbers[4])

# Negative indexing
print("Last element (negative):", numbers[-1])
print("Second to last:", numbers[-2])

# Empty list
empty = []
print("Empty list:", empty)

print()

print("=== Dictionary Tests ===")

# Create a dictionary
person = {"name": "Alice", "age": 30, "city": "New York"}
print("Person dict:", person)

# Access values
print("Name:", person["name"])
print("Age:", person["age"])
print("City:", person["city"])

# Empty dictionary
empty_dict = {}
print("Empty dict:", empty_dict)

print()

print("=== Mixed Data Tests ===")

# List with mixed types
mixed = [1, "hello", True, None]
print("Mixed list:", mixed)

# Dictionary with mixed values
complex_dict = {"number": 42, "text": "world", "flag": False}
print("Complex dict:", complex_dict)

# Nested structures
nested = [{"name": "Bob", "scores": [95, 87, 92]}, {"name": "Carol", "scores": [88, 91, 94]}]
print("Nested structure:", nested)

print()
print("=== All tests completed ===")
