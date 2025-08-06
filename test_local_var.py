# Test local function through variable
def local_test():
    print("Inside local_test function!")
    return 456

func_var = local_test
print("Before calling through variable")
result = func_var()
print("After calling through variable, result:", result)
