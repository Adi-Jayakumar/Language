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
 - [x] Implement struct inheritance
 - [x] Modules
 - [ ] Optimiser
 - [ ] Multiple file programs

# Long-term Goals
 - [ ] Allow methods in structs
 - [ ] Serialise and de-serialise RuntimeFunctions in order to seperate compilation and execution
 - [ ] Improve garbage collector heuristics to minimise loss of program execution time

# Known bugs TODO
 - [ ] Parsing arithmetic comparison statements like 'n < 10' and thinking it is a cast
 - [ ] Overloading of functions where the overloaded types are assignable to each other
 - [ ] Put parsed library functions into different container than normal functions

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
