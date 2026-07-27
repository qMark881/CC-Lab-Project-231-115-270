```c
int total;
int i;
bool passed;

total = 0;
i = 5;
passed = true;

while (i > 0) {
    total = total + i;
    i = i - 1;
}

if (passed && total >= 10) {
    print total;
} else {
    print i;
}
```
