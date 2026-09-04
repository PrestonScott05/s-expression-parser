#include "sexpr.h"



static void processChunk(const string &src) {
    Reader parser(src);
    try {
        while (parser.hasMoreStuff()) {
            print(eval(parser.read()));
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


