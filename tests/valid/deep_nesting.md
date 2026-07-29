```c
int x;
int y;
int z;
bool result;

x = 1;
y = 2;
z = 3;

if (x > 0) {
    if (y > 0) {
        if (z > 0) {
            result = true;
        } else {
            result = false;
        }
    } else {
        result = false;
    }
} else {
    result = false;
}

while (x > 0) {
    while (y > 0) {
        while (z > 0) {
            z = z - 1;
        }
        y = y - 1;
    }
    x = x - 1;
}
```