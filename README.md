# 🐱 Cat Language

Compile Atomic Text - a lightweight, statically typed language inspired by C/C++ with simplified syntax, expressive function names, and minimal boilerplate. Built to compile using LLVM.

## Features

*   **Statically Typed:** All variables must have a declared type.
*   **C/C++-style Declarations:** Familiar syntax for variable and function declarations.
*   **LLVM-based:** Compiles to LLVM IR for optimization and native code generation.
*   **Simplified Syntax:** Aims to be simpler than C++ with some playful custom keywords.

## Syntax

### Data Types and Variables

```cat
// Primitive Types
int     // Integer
float   // Floating point
string  // UTF-8 string
bool    // Boolean

// Variable Declarations
int x = 5;
float pi = 3.14;
string name = "Ami";
bool is_cat = true;
```

### Functions

```cat
// Function Definition
fn add(int a, int b) -> int {
    return a + b;
}

// Main Function (Entry Point)
fn main() -> int {
    print("Cat language running.\n");
    return 0;
}
```

### Input & Output

```cat
// Output
print("Hello, World!\n");   // Standard output (newline included)

// A special function for cat-like output.
meow("just a cat chilling\n");
// will output just a cat chilling

// Input
int age;
scan(age);
```

### Control Flow

```cat
// Conditionals
if (x > 0) {
    print("Positive\n");
} else {
    print("Non-positive\n");
}

// Loops
while (x < 10) {
    print(x);
    x = x + 1;
}
```

### Comments

```cat
// This is a single-line comment
```

## How to Build and Run

The included `run.bash` script will build and run your Cat programs.

```bash
bash run.bash
```

This will:
1.  Build the `cat` compiler.
2.  Compile `test/main.cat` into LLVM IR (`output.ll`).
3.  Compile the LLVM IR into a native executable (`my_program`).
4.  Run the `my_program` executable.

## Example Program

```cat
fn main() -> int {
    int a = 5;
    int b = 3;
    int c = add(a, b);

    print("Result: ");
    print(c);
    print("\n");

    return 0;
}

fn add(int x, int y) -> int {
    return x + y;
}
