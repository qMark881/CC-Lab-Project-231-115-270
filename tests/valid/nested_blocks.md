```c
int x;

x = 1;

if (x > 0) {
    int y;
    y = x + 1;

    {
        int z;
        z = y + 1;
        print z;
    }

    print y;
}
```