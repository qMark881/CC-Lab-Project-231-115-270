```c
int x;
int y;
float f;
bool b;

// Integer boundary tests
x = 0;
y = 0;

// Float boundary tests
f = 0.0;
f = 1.0;
f = -1.0;
f = 3.14159;

// Boolean boundary tests
b = true;
b = false;

// Assignment boundary tests
x = x;
y = y;

// Expression boundary tests
x = 0 + 0;
y = 1 - 1;
f = 1.0 * 1.0;
b = true && true;
b = false || false;

// Empty block test
{
    int temp;
    temp = 5;
}

// Multiple declarations
int a;
int b;
int c;
int d;
int e;

a = 1;
b = 2;
c = 3;
d = 4;
e = 5;
```