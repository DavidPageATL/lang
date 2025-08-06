# Simple try/except test
print("Testing simple try/except")

try:
    print("Before raise")
    raise("TestError")
except:
    print("Caught it!")

print("Done")
