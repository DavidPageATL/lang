# Simple test for list concatenation and for loops

print("=== List Concatenation Test ===")

# Test basic list concatenation
list1 = [1, 2, 3]
list2 = [4, 5, 6]
combined = list1 + list2
print("List1:", list1)
print("List2:", list2)
print("Combined:", combined)

print()

# Test list concatenation in for loop
print("Building list with for loop:")
results = []
numbers = [1, 2, 3, 4, 5]
for num in numbers:
    if num > 2:
        new_item = [num * 2]
        results = results + new_item
        print("Added:", num * 2, "Results:", results)

print("Final results:", results)

print()

# Test function with for loop and list operations
def process_numbers(nums):
    processed = []
    for n in nums:
        if n > 0:
            processed = processed + [n * n]
    return processed

test_nums = [1, -2, 3, -4, 5]
print("Original:", test_nums)
squared = process_numbers(test_nums)
print("Squared positives:", squared)

print()
print("=== Test completed ===")
