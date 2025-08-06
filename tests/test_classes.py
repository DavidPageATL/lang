# Test classes and objects

# Define a simple class
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def greet(self):
        print("Hello, my name is", self.name)
    
    def birthday(self):
        self.age = self.age + 1
        print(self.name, "is now", self.age, "years old")

# Create instances
p1 = Person("Alice", 25)
p2 = Person("Bob", 30)

# Test attribute access
print("Person 1:", p1.name, p1.age)
print("Person 2:", p2.name, p2.age)

# Test method calls
p1.greet()
p2.greet()

# Test method that modifies state
p1.birthday()
p2.birthday()
