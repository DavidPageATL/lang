# Test import system

# Test basic import
import math_utils
print("Testing basic import:")
print("math_utils.add(5, 3) =", math_utils.add(5, 3))
print("math_utils.PI =", math_utils.PI)

# Test import with alias
import math_utils as mu
print("\nTesting import with alias:")
print("mu.multiply(4, 7) =", mu.multiply(4, 7))

# Test from import
from math_utils import square, PI
print("\nTesting from import:")
print("square(6) =", square(6))
print("PI =", PI)

# Test from import with alias
from math_utils import add as plus
print("\nTesting from import with alias:")
print("plus(8, 2) =", plus(8, 2))
