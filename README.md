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
 - [x] Interfacing with C/C++ libraries
 - [x] Serialise and de-serialise Functions in order to seperate compilation and execution
 - [ ] Optimiser
    - [x] Evaluate constant expressions at compile time
    - [x] Propagate constants through program
    - [x] Keep performing optimisation passes until the AST does not change
    - [ ] Control flow optimisations
    - [ ] Peephole optimisations of bytecode
 - [ ] Multiple file programs
 - [ ] Verification
    - [x] Lex and parse verification statements
    - [x] Push precondition through bytecode for a subset of the language
    - [ ] Verify output

# Long-term Goals
 - [ ] Improve garbage collector heuristics to minimise loss of program execution time

# Known bugs TODO
 - [x] Overloading of functions where the overloaded types are assignable to each other
    - MWE:
        ```C
        function void Foo(int i){...}
        function void Foo(double d){...}

        Foo(3.14); // would have called the int version
                   // not the double version as it appears
                   // first and ints can be assigned to doubles 
        ```
 - [x] Put parsed library functions into different container than normal functions

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

# Verification

## Example 1
```C
function int Square(int x)
(|x = x_0|)
{
    return x * x;
    (|x_0 * x_0|)
}
```