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
int p;
int q;
int r;
int s;
int t;

p = 1;
q = 2;
r = 3;
s = 4;
t = 5;
```