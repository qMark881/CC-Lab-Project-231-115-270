```c
int x;

x = 10;

// This test checks for unterminated structures
// Note: The language doesn't have string literals, but this
// tests error handling for unexpected EOF

while (x > 0) {
    x = x - 1;
    // Missing closing brace will cause syntax error
```