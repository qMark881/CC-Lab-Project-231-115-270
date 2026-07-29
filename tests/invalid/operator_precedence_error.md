```c
int x;
int y;
bool b;

x = 10;
y = 20;

// This should trigger type errors due to invalid operator usage
b = x + y;  // Cannot assign int expression to bool variable
```