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

struct SExpression {
    ExpressionType type;
    string atomValue;

    shared_ptr<SExpression> car;
    shared_ptr<SExpression> cdr;
};

shared_ptr<SExpression> makeAtom(string value) {
    shared_ptr<SExpression> node = make_shared<SExpression>();
    node -> type = ExpressionType::Atom;
    node -> atomValue = move(value);
    node -> car = nullptr;
    node -> cdr = nullptr;

    return node;
}

shared_ptr<SExpression> makeNil() {
    shared_ptr<SExpression> node = make_shared<SExpression>();
    node->type = ExpressionType::Nil;
    node-> car = nullptr;
    node->cdr = nullptr;

    return node;
}

shared_ptr<SExpression> makePair(shared_ptr<SExpression> myCar, shared_ptr<SExpression> myCdr) {
    shared_ptr<SExpression> node = make_shared<SExpression>();
    node->type = ExpressionType::Pair;
    node->car = myCar;
    node-> cdr = myCdr;

    return node;
}

class Reader {
    public:
        explicit Reader(string str) : sourceString(move(str)), pos(0) {}

        shared_ptr<SExpression> read();
        bool hasMoreStuff();
    private:
        string sourceString;
        size_t pos = 0;

        void skipWhitespace();
        shared_ptr<SExpression> readList();
        shared_ptr<SExpression> readAtom();

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

        if (isspace(static_cast<unsigned char>(current)) || current == ',') {
            advance();
        } else {
            break;
        }
    }
}

bool Reader::hasMoreStuff() {
    skipWhitespace();
    return !atEnd();
}

shared_ptr<SExpression> Reader::read() {
    skipWhitespace();
    
    if (atEnd()) {
        throw runtime_error("we hit the end of the input unexpectedly");
    }

    char curr = peek();

    if (curr == ')') {
        throw runtime_error("unexpected ')' with no matching '('");
    }

    if (curr == '(') {
        return readList();
    } else {
        return readAtom();
    }
}

shared_ptr<SExpression> Reader::readAtom() {
    string result = "";

    while (!atEnd()) {
        char c = peek();
        if (c == ' ' || c == '\n' || c == '\t' || c == '\r' ||  c == '(' || c == ')' || c == ',') {
            break;
        }
        result += advance();
    }

    return makeAtom(result);
}

shared_ptr<SExpression> Reader::readList() {
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

void print(const shared_ptr<SExpression> &expr) {
    if (expr->type == ExpressionType::Atom) {
        cout << expr->atomValue;
    }
    else if (expr->type == ExpressionType::Nil) {
        cout << "()";
    }
    else {
        cout << "(";

        shared_ptr<SExpression> cur = expr;

        bool first = true;
        while (cur->type == ExpressionType::Pair) {
            if (!first) {
                cout << " ";
            }
            print(cur -> car);
            cur = cur->cdr;
            first = false;
        }
        cout << ")"; 
    }
}

static void processChunk(const string &src) {
    Reader parser(src);
    try {
        while (parser.hasMoreStuff()) {
            print(parser.read());
            cout << "\n";
        }
    } catch (const exception &ex) {
        cerr << "there was an error: " << ex.what() << endl;
    }
}

int main() {
    string pendingExpression;
    int currentDepth = 0;
    string line;

    while (getline(cin, line)) {
        pendingExpression += line;
        pendingExpression += '\n';

        for (char c : line) {
            if (c == '(') {
                currentDepth++;
            } else if (c == ')') {

                currentDepth--;
            }
        }

        if (currentDepth <= 0) {
            processChunk(pendingExpression);
            pendingExpression.clear();
            currentDepth = 0;
        }
    }

    if (!pendingExpression.empty()) {
        processChunk(pendingExpression);
    }

    return 0;
}

