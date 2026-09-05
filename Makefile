CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

repl: src/main.cpp src/sexpression.h
	$(CXX) $(CXXFLAGS) src/main.cpp -o repl

tests/run: tests/tests.cpp tests/doctest.h src/sexpression.h
	$(CXX) $(CXXFLAGS) -I src tests/tests.cpp -o tests/run


.PHONY: test clean
test: tests/run
	./tests/run


clean:
	rm -f repl tests/run