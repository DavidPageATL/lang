# Comprehensive test combining all features

print("=== Comprehensive Language Test ===")
print()

# Function definitions with for loops
def process_list(items):
    print("Processing list:", items)
    results = []
    for item in items:
        if item > 0:
            results = results + [item * 2]
    return results

def analyze_data(data_dict):
    print("Analyzing data...")
    total = 0
    count = 0
    for key in data_dict:
        value = data_dict[key]
        if value != None:
            total = total + value
            count = count + 1
    if count > 0:
        average = total / count
        return {"total": total, "count": count, "average": average}
    else:
        return {"total": 0, "count": 0, "average": 0}

# Test data
numbers = [1, -2, 3, 4, -5, 6]
scores = {"alice": 95, "bob": 87, "carol": 92, "dave": None, "eve": 88}

print("Original numbers:", numbers)
processed = process_list(numbers)
print("Processed numbers:", processed)
print()

print("Original scores:", scores)
stats = analyze_data(scores)
print("Statistics:", stats)
print()

# Nested data structures with functions
students = [
    {"name": "Alice", "grades": [95, 87, 92]},
    {"name": "Bob", "grades": [88, 91, 94]},
    {"name": "Carol", "grades": [76, 83, 89]}
]

print("Student grade analysis:")
for student in students:
    name = student["name"]
    grades = student["grades"]
    
    total = 0
    for grade in grades:
        total = total + grade
    
    average = total / 3
    print("Student:", name, "Average:", average)

print()

# Complex nested loops with conditions
print("Multiplication table (first 5):")
for i in [1, 2, 3, 4, 5]:
    row = []
    for j in [1, 2, 3, 4, 5]:
        product = i * j
        row = row + [product]
    print("Row", i, ":", row)

print()
print("=== All comprehensive tests completed ===")
