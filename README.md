# CMinus Compiler

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
  - [Compiling a CMinus Program](#compiling-a-cminus-program)
  - [Running the Compiler](#running-the-compiler)
- [Project Structure](#project-structure)

## Introduction

Welcome to the **CMinus Compiler** project! This compiler is an implementation of the CMinus programming language, a simplified subset of the C language designed for educational purposes. Developed as part of the Fall 2022 Compiler Construction course, this project demonstrates the fundamental concepts of compiler design, including lexical analysis, parsing, semantic analysis, and code generation.

## Features

- **Lexical Analysis:** Tokenizes the CMinus source code.
- **Parsing:** Constructs an Abstract Syntax Tree (AST) based on the grammar of CMinus.
- **Semantic Analysis:** Performs type checking and ensures semantic correctness.
- **Code Generation:** Generates intermediate code or target machine code.
- **Error Handling:** Provides meaningful error messages for syntax and semantic errors.
- **Modular Design:** Organized into clear modules for easy maintenance and extension.

## Getting Started

Follow these instructions to set up the CMinus Compiler on your local machine for development and testing purposes.

### Prerequisites

Ensure you have the following installed on your system:

- **C++ Compiler:** GCC or Clang
- **Flex:** Fast lexical analyzer generator
- **Bison:** Parser generator
- **Make:** Build automation tool
- **Git:** Version control system

### Installation

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/defene/2022fall-compiler_cminus.git
   cd 2022fall-compiler_cminus
   ```

2. **Build the Compiler:**

   ```bash
   make
   ```

This command will generate the executable for the CMinus Compiler.

## Usage

### Compiling a CMinus Program

To compile a CMinus source file, use the following command:

  ```bash
  ./cminus_compiler path/to/source.cminus -o output_file
  ```

### Running the Compiler

After building the compiler, you can run it with your CMinus source files. For example:

  ```bash
  ./cminus_compiler examples/hello.cminus -o hello
  ```

This will compile `hello.cminus` and produce an executable named `hello`.

## Project Structure

  ```makefile
  2022fall-compiler_cminus/
  ├── src/
  │   ├── lexer.l        # Flex lexer definition
  │   ├── parser.y       # Bison parser definition
  │   ├── ast.cpp        # Abstract Syntax Tree implementation
  │   ├── semantic.cpp   # Semantic analysis implementation
  │   ├── codegen.cpp    # Code generation implementation
  │   └── main.cpp       # Entry point of the compiler
  ├── include/
  │   ├── ast.h
  │   ├── semantic.h
  │   ├── codegen.h
  │   └── ...
  ├── examples/
  │   ├── hello.cminus
  │   └── factorial.cminus
  ├── Makefile
  └── README.md
```

- **src/:** Contains the source code for the compiler.
- **include/**: Header files for different modules.
- **examples/:** Sample CMinus programs to test the compiler.
- **Makefile:** Build configuration.
- **README.md:** Project documentation.
