TT
==
an implementation of my own language

```c
define fib(n) {
    if (n == 0 || n == 1) {
        1
    } else {
        fib(n-1) + fib(n-2)
    }
}

i = 0
while(i <= 10) {
    printf("%d\n", fib(i))
    i = i + 1
}
```
