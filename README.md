TTtL
==
TTtL stands for "TT the Language", which is an implementation of my own language, just for fun.

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

now TT supports require

```c
// in helper.tt
define add_3(a) {
    a+3
}

// in main.c
require("helper.tt")

printf("%d\n", add_3(20))
```

##TODO
supports for list
