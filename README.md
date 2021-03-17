# Language

An implementation of a C-style statically typed language that compiles down to a bytecode which is then interpreted.

# Progress
 - [x] Lex basic scripts
 - [x] Parse arithmetic expressions
 - [x] Lex and parse comparison expressions
 - [x] Lex and parse variables
 - [x] Parse statments
 - [x] Type check statements and expressions
 - [x] Compile things so far
 - [x] Compile and run if-else statements
 - [x] Compile and run functions
 - [x] Compile and run native functions
 - [x] Compile and run while loops
 - [x] Use Main function as entry point to program
 - [x] Implement arrays
 - [x] Implement strings
 - [x] Implement multidimensional arrays
 - [x] Implement structs
 - [ ] Allow methods in structs
 - [ ] Implement struct inheritance

# Long-term Goals
 - [ ] Optimiser
 - [ ] Multiple file programs
 - [ ] Modules

# Example Programs

Hello World!

```C
function void Main()
{
    Print("Hello World!");
}
```

A simple program to calculate the Fibbonacci numbers

```C
function int Fib(int n)
{
    if (n < 1)
        return n;
 
    return Fib(n - 1) + Fib(n - 2);
}

function void Main()
{
    Print(Fib(10));
}
```
