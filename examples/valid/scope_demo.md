```c
int x;
int y;

x = 10;
y = 20;

// Global scope
print x;
print y;

// Block with local scope
{
    int z;
    z = 30;
    print z;
    
    // Can access outer variables
    print x;
    
    // Local variable shadows global
    int x;
    x = 100;
    print x;
}

// Back to global scope
print x;

// Nested blocks
{
    int a;
    a = 5;
    {
        int b;
        b = 10;
        print a;
        print b;
    }
    // b is out of scope here
    print a;
}

// Final scope test
while (x > 0) {
    int temp;
    temp = x;
    print temp;
    x = x - 1;
}
```