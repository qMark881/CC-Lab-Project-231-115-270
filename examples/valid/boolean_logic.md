```c
bool a;
bool b;
bool result;

a = true;
b = false;

// Logical AND
result = a && b;
print result;

// Logical OR
result = a || b;
print result;

// Logical NOT
result = !a;
print result;

// Complex boolean expressions
result = (a && b) || (!a && !b);
print result;

result = (a || b) && (!a || !b);
print result;

// Boolean in conditions
if (a == true) {
    print 1;
}

if (b == false) {
    print 0;
}

// Boolean in loops
while (a == true) {
    print 1;
    a = false;
}
```