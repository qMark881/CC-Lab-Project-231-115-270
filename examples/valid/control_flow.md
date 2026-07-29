```c
int x;
int y;
bool condition;

x = 10;
y = 5;
condition = true;

// If statement
if (x > y) {
    print x;
}

// If-else statement
if (x < y) {
    print y;
} else {
    print x;
}

// While loop
while (x > 0) {
    print x;
    x = x - 1;
}

// Nested conditions
if (condition == true) {
    if (x == 0) {
        print 0;
    } else {
        print 1;
    }
}

// Complex condition
if (x > 0 && y > 0) {
    print 1;
}

if (x == 0 || y == 0) {
    print 0;
}
```