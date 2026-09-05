#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_POSIX_SIGNALS
#include "doctest.h"
#include "sexpression.h"

static shared_ptr<SExpression> parse(const string &str) {
    Reader reader(str);
    return reader.read();
}

static string run(const string &str) {
    Reader reader(str);
    return exprToString(eval(reader.read()));
}

TEST_SUITE("1. constructors and types and types") {
    TEST_CASE("1.1 makeAtom actually makes an atom") {
        auto a=makeAtom("x"); 
        CHECK(isAtom(a)); 
        CHECK(a->atomValue == "x");
    }

    TEST_CASE("1.2 makeNil actually makes nil") {
        CHECK(isNil(makeNil()));
    }

    TEST_CASE("1.3 makePair makes a pair") {
        shared_ptr<SExpression> pairToTest = makePair(makeAtom("a"), makeNil());
        CHECK(isPair(pairToTest));
    }
    TEST_CASE("1.4 make sure types are mutually exclusive") {
        shared_ptr<SExpression> a = makeAtom("a");
        CHECK(isAtom(a));
        CHECK_FALSE(isNil(a));
        CHECK_FALSE(isPair(a));
    }
}

TEST_SUITE("2. accessor functions (car/cdr/cons)") {
    TEST_CASE("2.1 car returns the car of the expression") {
        CHECK(exprToString(car(makePair(makeAtom("a"), makeAtom("b")))) == "a");
    }

    TEST_CASE("2.2 cdr returns the cdr") {
        CHECK(exprToString(cdr(makePair(makeAtom("a"), makeAtom("b")))) == "b");
    }

    TEST_CASE("2.3 const onto nil gives a one-element list") {
        CHECK(exprToString(cons(makeAtom("a"), makeNil())) == "(a)");
    }

    TEST_CASE("2.4 const of two atoms makes a dotted pair") {
        CHECK(exprToString(cons(makeAtom("a"), makeAtom("b"))) == "(a . b)");
    }
}

TEST_SUITE("reader tests - parsing only") {
    TEST_CASE("3.1 a simple atom") {
        CHECK(exprToString(parse("a")) == "a");
    }

    TEST_CASE("3.2 flat list") {
        CHECK(exprToString(parse("(a b c)")) == "(a b c)");
    }

    TEST_CASE("3.3 nested lists") { 
       CHECK(exprToString(parse("(a (b c) d)")) == "(a (b c) d)");
    }

    TEST_CASE("3.4 an empty list") {
        CHECK(exprToString(parse("()")) == "()");
    }

    TEST_CASE("3.5 'a expands to correct format") {
        CHECK(exprToString(parse("'a")) == "(quote a)");
    }

    TEST_CASE("3.6 '(a b) expands to the correct format") {
        CHECK(exprToString(parse("'(a b)")) == "(quote (a b))"); 
    }

    TEST_CASE("3.7 quote inside a list") {
        CHECK(exprToString(parse("(a 'b c)")) == "(a (quote b) c)");
    }

    TEST_CASE("3.8 an unexpected )") {
        CHECK_THROWS_AS(parse(")"), runtime_error);
    }

    TEST_CASE("3.9 error: unterminated expression") {
        CHECK_THROWS_AS(parse("(a b"), runtime_error);
    }
}

TEST_SUITE("4. evaluation tests (operators)") {
    TEST_CASE("4.1 atom self-eval") {
        CHECK(run("a") == "a");
    }

    TEST_CASE("4.2 nil self-eval") {
        CHECK((run("()")) == "()");
    }

    TEST_CASE("4.3 quote does NOT evaluate its argument") {
        CHECK(run("(quote (car x))") == "(car x)");
    }

    TEST_CASE("4.4 car") {
        CHECK(run("(car (quote (a b c)))") == "a");
    }

    TEST_CASE("4.5 cdr") {
        CHECK(run("(cdr (quote (a b c)))") == "(b c)");
    }

    TEST_CASE("4.6 cons makes a list") {
        CHECK(run("(cons a ())") == "(a)");
    }

    TEST_CASE("4.7 cons making a nested list") {
        CHECK(run("(cons a (cons b ()))") == "(a b)");
    }

    TEST_CASE("4.8 cons with dotted notattion") {
        CHECK(run("(cons a b)") == "(a . b)");
    }

    TEST_CASE("4.9 evaluation runs as an evaluated argument") {
        CHECK(run("(eval (car (quote (a b c))))") == "a");
    }

    TEST_CASE("4.10 eval: cdr result self-evaluates recursively") {
        CHECK(run("(eval (cdr (quote (a b c))))") == "(b c)");
    }

    TEST_CASE("4.11 eval runs quoted code") {
        CHECK(run("(eval (quote (car (quote (a b c)))))") == "a");
    }

    TEST_CASE("4.12 shorthand quote produces same result as long version") {
        CHECK(run("(car '(a b c))") == run("(car (quote (a b c)))"));
    }

    TEST_CASE("4.13 unknown symbol just returns itself") {
        CHECK(run("(bogus 'a)") == "(bogus (quote a))");
    }

    TEST_CASE("4.14 non-atom head just returns data") {
        CHECK(run("((quote a) 'b)") == "((quote a) (quote b))");
    }
}

TEST_SUITE("5.4 printing") {
    TEST_CASE("5.1 atom") {
        CHECK(exprToString(makeAtom("hi")) == "hi");
    }

    TEST_CASE("5.2 nil") {
        CHECK(exprToString(makeNil()) == "()");
    }

    TEST_CASE("5.3 flat list") {
        CHECK(exprToString(parse("(a b c)")) == "(a b c)");
    }

    TEST_CASE("5.4 nested list") {
        CHECK(exprToString(parse("(a (b) c)")) == "(a (b) c)");
    }

    TEST_CASE("5.5 dotted pair") {
        CHECK(exprToString(cons(makeAtom("a"), makeAtom("b"))) == "(a . b)");
    }
}

TEST_SUITE("6. project 1.1") {
    TEST_CASE("6.1 empty list") {
        CHECK(run("()") == "()");   
    }

    TEST_CASE("6.2 atoms") {
        CHECK(run("a")=="a");
        CHECK(run("45") == "45");
        CHECK(run("test") == "test");
    }

    TEST_CASE("6.3 flat number list") {
        CHECK(run("(1 2 3)") == "(1 2 3)");
    }

    TEST_CASE("6.4 list of lists") {
       CHECK(run("((1 2 3) (a b c))") == "((1 2 3) (a b c))");
    }

    TEST_CASE("6.5 nested empty lists") {
        CHECK(run("( ( ) ( ) ( ) )") == "(() () ())");
    }

    TEST_CASE("6.6 the big example") {
        CHECK(run("(() a 45 test (1 2 3) ((1 2 3) (a b c) ) ( ( ) ( ) ( ) ))") == "(() a 45 test (1 2 3) ((1 2 3) (a b c)) (() () ()))");
    }

    TEST_CASE("6.7 non-operator lsit stays the same") {
        CHECK(run("(b c)") == "(b c)");
    }
}

TEST_SUITE("7. project 1.2 spec examples") {
    TEST_CASE("7.1 car") { 
        CHECK(run("(car (quote (a b c)))") == "a");
    }

    TEST_CASE("7.2 cdr") { 
        CHECK(run("(cdr (quote (a b c)))") == "(b c)"); 
    }

    TEST_CASE("7.3 eval of car") { 
        CHECK(run("(eval (car (quote (a b c))))") == "a"); 
    }

    TEST_CASE("7.4 eval of cdr") { 
        CHECK(run("(eval (cdr (quote (a b c))))") == "(b c)"); 
    }
    TEST_CASE("7.5 cons -> dotted") { 
        CHECK(run("(cons a b)") == "(a . b)"); 
    }
    TEST_CASE("7.6 cons onto nil")  { 
        CHECK(run("(cons a ())") == "(a)"); 
    }

    TEST_CASE("7.7 cons builds list"){ 
        CHECK(run("(cons a (cons b ()))") == "(a b)"); 
    }

    TEST_CASE("7.8 quote of atom") { 
        CHECK(run("(quote x)") == "x"); 
    }

    TEST_CASE("7.9 'a shorthand") { 
        CHECK(run("'a") == "a"); 
    }
    
    TEST_CASE("7.10 '(car x) shorthand") { 
        CHECK(run("'(car x)") == "(car x)"); 
    }
}