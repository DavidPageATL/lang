# Test file for all the new built-in functions

# Test len() function
print("=== Testing len() ===")
my_list = [1, 2, 3, 4, 5]
print("len([1, 2, 3, 4, 5]):", len(my_list))

my_dict = {"a": 1, "b": 2, "c": 3}
print("len({'a': 1, 'b': 2, 'c': 3}):", len(my_dict))

my_string = "hello"
print("len('hello'):", len(my_string))

# Test range() function
print("\n=== Testing range() ===")
print("range(5):", range(5))
print("range(2, 8):", range(2, 8))
print("range(0, 10, 2):", range(0, 10, 2))
print("range(10, 0, -2):", range(10, 0, -2))

# Test str() function
print("\n=== Testing str() ===")
print("str(42):", str(42))
print("str(3.14):", str(3.14))
print("str(True):", str(True))

# Test int() function
print("\n=== Testing int() ===")
print("int(3.14):", int(3.14))
print("int('42'):", int('42'))
print("int(5.99):", int(5.99))

# Test float() function
print("\n=== Testing float() ===")
print("float(42):", float(42))
print("float('3.14'):", float('3.14'))
print("float(7):", float(7))

# Test type() function
print("\n=== Testing type() ===")
print("type(42):", type(42))
print("type('hello'):", type('hello'))
print("type([1, 2, 3]):", type([1, 2, 3]))
print("type({'a': 1}):", type({'a': 1}))
print("type(True):", type(True))

# Test bool() function
print("\n=== Testing bool() ===")
print("bool(42):", bool(42))
print("bool(0):", bool(0))
print("bool('hello'):", bool('hello'))
print("bool(''):", bool(''))
print("bool([1, 2]):", bool([1, 2]))
print("bool([]):", bool([]))

# Test min() function
print("\n=== Testing min() ===")
print("min(5, 2, 8, 1, 9):", min(5, 2, 8, 1, 9))
print("min([5, 2, 8, 1, 9]):", min([5, 2, 8, 1, 9]))

# Test max() function
print("\n=== Testing max() ===")
print("max(5, 2, 8, 1, 9):", max(5, 2, 8, 1, 9))
print("max([5, 2, 8, 1, 9]):", max([5, 2, 8, 1, 9]))

# Test sum() function
print("\n=== Testing sum() ===")
print("sum([1, 2, 3, 4, 5]):", sum([1, 2, 3, 4, 5]))
print("sum([1, 2, 3], 10):", sum([1, 2, 3], 10))

# Test with try/except to show error handling works
print("\n=== Testing error handling ===")
try:
    len(42)
except Exception as e:
    print("Error caught:", e)

try:
    int("not_a_number")
except Exception as e:
    print("Error caught:", e)
