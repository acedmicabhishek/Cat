# 🐱 Cat Language Compiler - Implemented Features

This document summarizes the features that have been successfully implemented in the Cat language compiler.

## 1. Project and Build System

- **Build System**: The project uses `CMake` for robust and cross-platform builds.
- **Build Script**: A `run.bash` script is provided to automate the build process (clean, configure, compile) and run the compiler on a test file.
- **Source Control**: A `.gitignore` file is included to exclude build artifacts.

## 2. Lexer (Tokenizer)

The lexer is responsible for scanning the raw source code and converting it into a stream of tokens.

- **Keywords**: `fn`, `return`, `if`, `else`, `while`, `int`, `float`, `string`, `print`, `bool`, `true`, `false`, `scan`.
- **Literals**: Integer literals, float literals, string literals, boolean literals.
- **Identifiers**: Variable and function names.
- **Operators**: `+`, `-`, `*`, `=`, `>`, `->`, `:`, `==`, `!=`, `<=`, `>=`, `&&`, `||`, `!`.
- **Punctuation**: `(`, `)`, `{`, `}`, `;`, `,`.
- **Whitespace & Comments**: Correctly handles and skips whitespace, newlines, and single-line comments (`//`).

## 3. Parser (Syntax Analysis)

The parser takes the token stream from the lexer and builds an Abstract Syntax Tree (AST) representing the program's structure.

- **Module Parsing**: Can parse a file containing multiple function definitions.
- **Function Definitions**: Parses complete function definitions, including:
    - Function name.
    - Typed arguments (e.g., `int x`).
    - Return type declaration (e.g., `-> int` or `: int`).
    - A body enclosed in curly braces `{}`.
- **Statement Parsing**:
    - **Variable Declarations**: `int a = 5;`
    - **Return Statements**: `return 0;`
    - **Print Statements**: `print("Hello");`
    - **Scan Statements**: `scan(x);`
    - **If-Else Statements**: `if (condition) { ... } else { ... }`
    - **While Loops**: `while (condition) { ... }`
- **Expression Parsing**:
    - **Literals**: Numbers, strings, and booleans.
    - **Variable References**: `a`.
    - **Function Calls**: `add(a, b)`.
    - **Binary Operations**: A basic operator-precedence parser is in place to handle expressions like `x + y`, `x == y`, `x && y`.
    - **Unary Operations**: `!x`.

## 4. Code Generator (LLVM IR)

The code generator traverses the AST and emits LLVM Intermediate Representation (IR).

- **Two-Pass Generation**: Uses a two-pass approach (prototypes first, then bodies) to correctly handle function calls before their definition.
- **Function Generation**: Creates LLVM functions with correct signatures, including typed arguments and return values.
- **Variable Handling**: Uses `alloca` to create stack space for local variables and function arguments.
- **Instruction Generation**:
    - Correctly generates integer (`add`, `sub`, `mul`) and floating-point (`fadd`, `fsub`, `fmul`) instructions based on operand types.
    - Generates `store` and `load` instructions for variable access.
    - Generates `call` instructions for function invocation.
    - Generates `ret` instructions for returning from functions.
    - Generates conditional branching (`icmp`, `br`) for `if-else` statements.
    - Generates logical operations (`and`, `or`, `not`).
- **External Functions**: Declares external C functions like `print` (for strings), `print_int` and `scan_int` for interoperability.