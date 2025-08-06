# Test with side effects
counter = 0

def increment():
    global counter
    counter = counter + 1
    return counter
