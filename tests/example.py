# Sample Python-like program
# This demonstrates the parser capabilities

# Variable assignments
name = "Alice"
age = 25
height = 5.6
is_student = True

# Print statements
print("Personal Information:")
print("Name:", name)
print("Age:", age)
print("Height:", height)
print("Is student:", is_student)

# Arithmetic operations
birth_year = 2024 - age
print("Birth year:", birth_year)

# Conditional statements
if age >= 18:
    print("You are an adult")
    if age >= 65:
        print("You are a senior citizen")
    else:
        print("You are a working-age adult")
else:
    print("You are a minor")

# Loop example
print("\nCounting to 5:")
count = 1
while count <= 5:
    print("Count:", count)
    count = count + 1

print("\nProgram completed!")
