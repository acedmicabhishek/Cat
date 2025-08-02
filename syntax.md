# 🐱 Cat Language v0.1 – Syntax Specification

Cat is a lightweight, statically typed language inspired by C/C++ with simplified syntax, expressive function names, and minimal boilerplate. Built to compile using LLVM.

---

## Extension
`file.cat`

## Core Concepts

- Statically typed
- C/C++-style declarations
- LLVM-based compiler backend
- Simpler than C++, with some playful custom keywords

---

## Data Types and Variables

### Primitive Types
```cat
int     // Integer
float   // Floating point
string  // UTF-8 string
```

### Variable Declarations
```cat
int x = 5;
float pi = 3.14;
string name = "Ami";
```

---

## Functions

### Function Definition
```cat
fn add(int a, int b) -> int {
    return a + b;
}
```

### Main Function (Entry Point)
The `main` function is the entry point for any Cat program.
```cat
fn main() : int {
    print("Cat language running.");
    return 0;
}
```

---

## Input & Output

### Output
```cat
print("Hello, World!");   // Standard output (newline included)
```

### meow
A special function for cat-like output.
```cat
meow("just a cat chilling");
// will output just a cat chilling
```

### Input
```cat
int age;
scan(age);
```

---

## Control Flow

### Conditionals
```cat
if (x > 0) {
    print("Positive");
} else {
    print("Non-positive");
}
```

### Loops
```cat
while (x < 10) {
    print(x);
    x = x + 1;
}
```

---

## Comments
```cat
// This is a single-line comment
```

---

## Reserved Keywords (v0.1)
```
fn, return, if, else, while, int, float, string, print, scan, meow, main
```

---

## Planned Features (Later Versions)

| Feature       | Syntax Idea             | Description            |
|---------------|-------------------------|------------------------|
| Type inference| `var x = 10;`           | Optional keyword-based |
| Arrays        | `int nums[5];`          | C-style arrays         |
| Structs       | `struct Cat { int paws; }`| C-like structs         |
| String concat | `"hi " + name`          | Operator overloading   |
| File I/O      | `file = open("data.txt");`| File operations        |

---

## Reserved Keywords (v0.1)
```fn, return, if, else, while, int, float, string, print, scan, meow, main
```

## Example Program
```cat
fn main() : int {
    int a = 5;
    int b = 3;
    int c = add(a, b);

    print("Result:");
    print(c);

    return 0;
}

fn add(int x, int y) : int {
    return x + y;
}
