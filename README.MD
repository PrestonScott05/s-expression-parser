# S-Expression Reader

This parser, which I will call Project 1, built for Programming Languages (CS-403) at The University of Alabama.

## Project layout

```
src/
  sexpression.h   the interpreter: reader, eval, and printer (header-only)
  main.cpp        the REPL -- reads stdin, evaluates, prints
tests/
  tests.cpp       the doctest test suite
  doctest.h       the doctest framework (vendored, unmodified)
Makefile
```

The interpreter itself has no external dependencies and needs only a C++17 compiler
(I developed with `g++`). The test suite uses [doctest](https://github.com/doctest/doctest),

a single-header framework that is checked into the repo at `tests/doctest.h`, so there is
nothing to install for it either.

## Building

With `make`, from the project root:

```
make          # builds the project -> ./repl
make test     # builds and runs the test suite
make clean    # removes the built binaries
```

Without `make`, the two commands the Makefile runs are:

```
g++ -std=c++17 -o repl src/main.cpp
g++ -std=c++17 -I src tests/tests.cpp -o tests/run && ./tests/run
```

The `-I src` on the test build is what lets `tests/tests.cpp` find `sexpression.h`
while living in a different directory.

## Usage

The program reads all of standard input, so the most reliable way to run it is to pipe
input in or redirect a file, which ends the input automatically:

```
echo '(1 2 3)' | ./repl
./repl < tests.txt
```

You can also run it interactively:

```
./repl
```

Type expressions (they can span multiple lines, and there may be several per line), then
signal end-of-input with **Ctrl-D on an otherwise empty line**. On malformed input it
prints an error message and continues.

## Testing

`make test` compiles and runs the suite; a passing run ends with a line like `54 passed`.
The tests are grouped by unit (constructors, accessors, reader, eval, printing), plus one
suite per project part: 1.1 read/print round-trips and 1.2 eval spec examples.

### Building the tests on Cygwin

doctest auto-enables a POSIX signal handler that Cygwin's headers don't fully support,
which breaks the test build. `tests.cpp` disables it with a define before the include:

```cpp
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_POSIX_SIGNALS
#include "doctest.h"
```