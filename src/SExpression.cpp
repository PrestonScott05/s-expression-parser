#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <cctype>
#include <stdexcept>
#include <sstream>

enum class ExpressionType {
    Nil,
    Atom, 
    Pair
};

struct SExpression {
    ExpressionType type;
    std::string atomValue;

    std::shared_ptr<SExpression> car;
    std::shared_ptr<SExpression> cdr;
};

std::shared_ptr<SExpression> makeAtom(std::string value) {
    std::shared_ptr<SExpression> node = std::make_shared<SExpression>();
    node -> type = ExpressionType::Atom;
    node -> atomValue = std::move(value);
    node -> car = nullptr;
    node -> cdr = nullptr;

    return node;
}

std::shared_ptr<SExpression> makeNil() {
    std::shared_ptr<SExpression> node = std::make_shared<SExpression>();
    node->type = ExpressionType::Nil;
    node-> car = nullptr;
    node->cdr = nullptr;

    return node;
}

std::shared_ptr<SExpression> makePair(std::shared_ptr<SExpression> myCar, std::shared_ptr<SExpression> myCdr) {
    std::shared_ptr<SExpression> node = std::make_shared<SExpression>();
    node->type = ExpressionType::Pair;
    node->car = myCar;
    node-> cdr = myCdr;

    return node;
}

class Reader {
    public:
        explicit Reader(std::string str) : sourceString(std::move(str)), pos(0) {}

        std::shared_ptr<SExpression> read();
        bool hasMoreStuff();
    private:
        std::string sourceString;
        size_t pos = 0;

        void skipWhitespace();
        std::shared_ptr<SExpression> readList();
        std::shared_ptr<SExpression> readAtom();

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

void Reader::skipWhitespace() {
    while (!atEnd()) {
        char current = peek();

        if (!std::isspace(static_cast<unsigned char>(current))) {
            break;
        } else {
            advance();
        }
    }
}

bool Reader::hasMoreStuff() {
    skipWhitespace();
    return !atEnd();
}

std::shared_ptr<SExpression> Reader::read() {
    skipWhitespace();
    
    if (atEnd()) {
        throw std::runtime_error("we hit the end of the input unexpectedly");
    }

    char curr = peek();

    if (curr == '(') {
        return readList();
    } else {
        return readAtom();
    }
}

std::shared_ptr<SExpression> Reader::readAtom() {
    std::string result = "";

    while (!atEnd()) {
        char c = peek();
        if (c == ' ' || c == '\n' || c == '\t' || c == '\r' ||  c == '(' || c == ')') {
            break;
        }
        result += advance();
    }

    return makeAtom(result);
}

std::shared_ptr<SExpression> Reader::readList() {
    //eat openening parentheses
    advance();

    std::vector<std::shared_ptr<SExpression>> elements;


    while (true) {
        skipWhitespace();

        if (atEnd()) {
            throw std::runtime_error("you didn't terminate the list. Missing: ')'");
        }

        if (peek() == ')') {
            advance();
            break;
        }

        elements.push_back(read());
    }

    std::shared_ptr<SExpression> list = makeNil();

    for (auto iterator = elements.rbegin(); iterator != elements.rend(); iterator++) {
        list = makePair(*iterator, list);
    }

    return list;
}

void print(const std::shared_ptr<SExpression> &expr) {
    if (expr->type == ExpressionType::Atom) {
        std::cout << expr->atomValue;
    }
    else if (expr->type == ExpressionType::Nil) {
        std::cout << "()";
    }
    else {
        std::cout << "(";

        std::shared_ptr<SExpression> cur = expr;

        bool first = true;
        while (cur->type == ExpressionType::Pair) {
            if (!first) {
                std::cout << " ";
            }
            print(cur -> car);
            cur = cur->cdr;
            first = false;
        }
        std::cout << ")"; 
    }
}

int main() {
    std::stringstream buffer;
    buffer << std::cin.rdbuf();

    Reader parser(buffer.str());

    try {
        while (parser.hasMoreStuff()) {
            print(parser.read());
            std::cout << "\n";
        }
    } catch (const std::exception &ex) {
        std::cerr << "there was an error: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}

