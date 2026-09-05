#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <cctype>
#include <stdexcept>
#include <sstream>

using namespace std;

enum class ExpressionType {
    Nil,
    Atom, 
    Pair
};

enum class Operator {
    Quote,
    Car,
    Cdr,
    Cons,
    Eval,
    Unknown 
};

struct SExpression {
    ExpressionType type;
    string atomValue;

    shared_ptr<SExpression> car;
    shared_ptr<SExpression> cdr;
};

inline shared_ptr<SExpression> makeAtom(string value) {
    shared_ptr<SExpression> node = make_shared<SExpression>();
    node -> type = ExpressionType::Atom;
    node -> atomValue = move(value);
    node -> car = nullptr;
    node -> cdr = nullptr;

    return node;
}

inline shared_ptr<SExpression> makeNil() {
    shared_ptr<SExpression> node = make_shared<SExpression>();
    node->type = ExpressionType::Nil;
    node-> car = nullptr;
    node->cdr = nullptr;

    return node;
}

inline shared_ptr<SExpression> makePair(shared_ptr<SExpression> myCar, shared_ptr<SExpression> myCdr) {
    shared_ptr<SExpression> node = make_shared<SExpression>();
    node->type = ExpressionType::Pair;
    node->car = myCar;
    node-> cdr = myCdr;

    return node;
}

inline shared_ptr<SExpression> car(shared_ptr<SExpression> cell) {
    return cell->car;
}

inline shared_ptr<SExpression> cdr(shared_ptr<SExpression> cell) {
    return cell->cdr;
}

inline shared_ptr<SExpression> cons(shared_ptr<SExpression> a, shared_ptr<SExpression> b) {
    return makePair(a, b);
}

inline bool isNil(shared_ptr<SExpression> node) {
    return node->type == ExpressionType::Nil;
}

inline bool isAtom(shared_ptr<SExpression> node) {
    return node->type == ExpressionType::Atom;
}

inline bool isPair(shared_ptr<SExpression> node) {
    return node->type == ExpressionType::Pair;
}

class Reader {
    public:
        explicit Reader(string str) : sourceString(move(str)), pos(0) {}

        shared_ptr<SExpression> read() {
            skipWhitespace();
            
            if (atEnd()) {
                throw runtime_error("we hit the end of the input unexpectedly");
            }

            char curr = peek();

            if (curr == ')') {
                throw runtime_error("unexpected ')' with no matching '('");
            }
            
            if (curr == '\'') {
                advance();
                shared_ptr<SExpression> quoted = read();
                shared_ptr<SExpression> result = makePair(makeAtom("quote"), makePair(quoted, makeNil()));

                return result;
            }

            if (curr == '(') {
                return readList();
            } else {
                return readAtom();
            }
        }

        bool hasMoreStuff() {
            skipWhitespace();
            return !atEnd();
        }

    private:
        string sourceString;
        size_t pos = 0;

        void skipWhitespace() {
            while (!atEnd()) {
                char current = peek();

                if (isspace(static_cast<unsigned char>(current)) || current == ',') {
                    advance();
                } else {
                    break;
                }
            }
        }

        shared_ptr<SExpression> readAtom() {
            string result = "";

            while (!atEnd()) {
                char c = peek();
                if (c == ' ' || c == '\n' || c == '\t' || c == '\r' ||  c == '(' || c == ')' || c == ',' || c == '\'') {
                    break;
                }
                result += advance();
            }

            return makeAtom(result);
        }

        shared_ptr<SExpression> readList() {
            //eat openening parentheses
            advance();

            vector<shared_ptr<SExpression>> elements;


            while (true) {
                skipWhitespace();

                if (atEnd()) {
                    throw runtime_error("you didn't terminate the list. Missing: ')'");
                }

                if (peek() == ')') {
                    advance();
                    break;
                }

                elements.push_back(read());
            }

            shared_ptr<SExpression> list = makeNil();

            for (auto iterator = elements.rbegin(); iterator != elements.rend(); iterator++) {
                list = makePair(*iterator, list);
            }

            return list;
        }

        char peek() {
            if (atEnd()) {
                return '\0';
            }
            else {
                return sourceString[pos];
            }
        }

        char advance() {
            char c = sourceString[pos];
            pos++ ;
            return c;
        }

        bool atEnd() const {
            if (pos >= sourceString.size()) {
                return true;
            }
            else {
                return false;
            }
        }
};

inline string exprToString(const shared_ptr<SExpression> &expr) {
    if (isAtom(expr)) return expr->atomValue;
    if (isNil(expr)) return "()";

    string out = "(";
    auto cur = expr;
    bool first = true;

    while (isPair(cur)) {
        if (!first) {
            out += " ";
        }

        out += exprToString(cur->car);
        cur = cur->cdr;
        first = false;
    }

    if (!isNil(cur)) {
        out += " . " + exprToString(cur);
    }

    return out + ")";
}

inline void print(const shared_ptr<SExpression> &expr) {
    cout << exprToString(expr);
}

inline Operator toOp(const string &s) {
    if (s == "quote") return Operator::Quote;
    if (s == "car")   return Operator::Car;
    if (s == "cdr")   return Operator::Cdr;
    if (s == "cons")  return Operator::Cons;
    if (s == "eval")  return Operator::Eval;
    return Operator::Unknown;
}

inline shared_ptr<SExpression> eval(shared_ptr<SExpression> expression) {
    if (isAtom(expression)) {
        return expression;
    }

    if (isNil(expression)) {
        return expression;
    }

    shared_ptr<SExpression> operation = car(expression);
    if (!isAtom(operation)) {
        return expression;
    }

    shared_ptr<SExpression> first_part = car(cdr(expression));
    shared_ptr<SExpression> second_part = car(cdr(cdr(expression)));

    string operation_string = operation->atomValue;
    switch (toOp(operation_string))
    {
        case Operator::Quote: return first_part;
        case Operator::Car: return car(eval(first_part));
        case Operator::Cdr: return cdr(eval(first_part));
        case Operator::Cons: return cons(eval(first_part), eval(second_part));
        case Operator::Eval: return eval(eval(first_part));
        case Operator::Unknown: return expression;
    }
    throw runtime_error("eval: unreachable");
}
  

