```c
bool a;
bool b;
bool c;

a = true;
b = false;
c = !(a && b) || (a == true);

if (c && a) {
    print a;
} else {
    print b;
}
```