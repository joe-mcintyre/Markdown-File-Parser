# Markdown File Compiler

Markdown File Compiler is a C-based tool that converts Markdown files into HTML and hosts them using a local web socket.

## Features
* Markdown to HTML conversion
* Automatic styling
* Local web hosting
* Lightweight and fast

## Installation
1. Clone the repository:
   ```sh
   git clone https://github.com/joe-mcintyre/Markdown-File-Parser.git
   cd Markdown-File-Parser
   ```
2. Compile the program:
   ```sh
   make clean
   make
   ```

## Usage
1. Run the compiler using the target markdownfile as the 1st argument:
   ```sh
   ./md_parser EXAMPLE_MD.md
   ```
2. Open a browser and visit:
   ```
   http://localhost:8080
   ```

## Requirements
- GCC Compiler
- Makefile
- Basic C Libraries
