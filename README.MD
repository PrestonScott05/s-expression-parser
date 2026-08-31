# S-Expression Reader

This parser, which I will call Project 1, built for Programming Languages (CS-403) at The University of Alabama. 

## Setup

This project is a single c++ source file and requires only a c++17-capable compiler (I developed with `g++`)

```
g++ -std=c++17 -o parser src/SExpression.cpp
```

There are no external dependencies. 

## Usage

The program reads all of the standard input, so the most reliable way to run it is to pipe input in or redirect a file, which ends the input automatically and continues the rest of the program.

```
echo '(1 2 3)' | ./parser
./parser < tests.txt
```

You can also run it interactively:

```
./parser
```

Type expressions (they can span multiple lines and there may be several per line), then signal end-of-input. This is **Ctrl-D on an otherwise empty line**  

On a malformed input, it prints an error message. 
