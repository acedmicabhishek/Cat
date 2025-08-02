# CatLang Documentation

Welcome to the official documentation for CatLang, a simple, statically-typed programming language.

## 1. Introduction

CatLang is a programming language built with a C++ compiler using LLVM. It's designed to be simple and easy to learn, with a C-like syntax.

## 2. Syntax

### 2.1. Data Types

CatLang supports the following basic data types:

*   `int`: 32-bit signed integers (e.g., `5`, `-10`).
*   `float`: 32-bit floating-point numbers (e.g., `3.14`).
*   `bool`: Boolean values, `true` or `false`.
*   `string`: String literals (e.g., `"hello"`).

### 2.2. Variables

Variables are declared with their type, followed by the variable name. They can be optionally initialized at declaration.

```cat
// Declare an integer variable
int x;

// Declare and initialize a float variable
float y = 3.14;

// Declare a boolean variable
bool is_cat = true;
```

### 2.3. Functions

Functions are defined using the `fn` keyword. You must specify the types of the arguments and the return type.

```cat
// A function that takes two integers and returns their sum
fn add(int a, int b) -> int {
    return a + b;
}

// A function with no return value
fn greet(string name) -> void {
    print("Hello, ");
    print(name);
}
```

Functions can be called from anywhere in the code, as CatLang supports forward declaration automatically.

### 2.4. Control Flow

#### If-Else Statements

CatLang supports `if` and `if-else` statements for conditional execution.

```cat
int x = 10;
if (x > 5) {
    print("x is greater than 5");
} else {
    print("x is not greater than 5");
}
```

#### While Loops

`while` loops are used for repeated execution of a block of code.

```cat
int i = 0;
while (i < 5) {
    print(i);
    i = i + 1;
}
```

### 2.5. Operators

CatLang supports standard arithmetic, comparison, and logical operators.

*   **Arithmetic:** `+`, `-`, `*`
*   **Comparison:** `<`, `==`, `!=`, `<=`, `>=`
*   **Logical:** `&&` (and), `||` (or), `!` (not)

### 2.6. Built-in Functions

#### `print()`

The `print()` function is used to output values to the console. It can print string literals, variables, and expressions.

```cat
print("Hello, World!\n");

int x = 42;
print(x); // Prints the value of x

print(5 + 3); // Prints the result of the expression
```

#### `scan()`

The `scan()` function is used to read input from the user and store it in a variable. It automatically detects the variable's type and reads the corresponding value (e.g., integer or float).

```cat
int my_var;
print("Enter a number: ");
scan(my_var);
print("You entered: ");
print(my_var);
```

## 3. How to Compile and Run

The provided `run.bash` script automates the compilation and execution process.

1.  **Write your CatLang code** in a file (e.g., `test/main.cat`).
2.  **Run the script:**
    ```bash
    ./run.bash
    ```

The script will:
1.  Build the `cat` compiler using CMake and Make.
2.  Use the compiler to generate LLVM IR (`output.ll`).
3.  Compile the LLVM IR into an executable (`my_program`) using `clang`.
4.  Run the final executable.

## 4. Example Program

Here is a complete example program that demonstrates several features of CatLang:

```cat
fn main() -> int {
    int c = add(5, 3);
    print(c);
    return 0;
}

fn add(int x, int y) -> int {
    return x + y;
}
