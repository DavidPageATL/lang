# Comprehensive test of our Python-like language
print("=== Testing Language Features ===")

# Variables and basic types
name = "Python-like Language"
version = 1.0
features = ["variables", "functions", "classes", "lists", "dicts"]
active = True

print("Language:", name, "Version:", version)
print("Features:", features)
print("Active:", active)

# Functions
def fibonacci(n):
    if n <= 1:
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)

print("Fibonacci of 6:", fibonacci(6))

# Lists and dictionaries
numbers = [1, 2, 3, 4, 5]
squares = []
for num in numbers:
    squares = squares + [num * num]

print("Numbers:", numbers)
print("Squares:", squares)

student_grades = {"Alice": 85, "Bob": 92, "Charlie": 78}
print("Grades:", student_grades)

for name in student_grades:
    print(name, "scored", student_grades[name])

# Classes and objects
class BankAccount:
    def __init__(self, owner, balance):
        self.owner = owner
        self.balance = balance
        print("Account created for", owner, "with balance", balance)
    
    def deposit(self, amount):
        self.balance = self.balance + amount
        print(self.owner, "deposited", amount, "New balance:", self.balance)
    
    def withdraw(self, amount):
        if amount <= self.balance:
            self.balance = self.balance - amount
            print(self.owner, "withdrew", amount, "New balance:", self.balance)
        else:
            print("Insufficient funds for", self.owner)
    
    def get_balance(self):
        return self.balance

# Test the bank account class
account1 = BankAccount("Alice", 1000)
account2 = BankAccount("Bob", 500)

account1.deposit(200)
account1.withdraw(150)
account2.withdraw(600)  # Should fail

print("Alice's final balance:", account1.get_balance())
print("Bob's final balance:", account2.get_balance())

print("=== All tests completed! ===")
