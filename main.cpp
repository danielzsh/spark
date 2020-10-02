#include <iostream>
#include <fstream>
#include "Parser/parser.h"
#include "Lexer/Lexer.h"
#include "Interpreter/interpreter.h"

// Tip: Don't use using namespace, see https://bit.ly/aaron_help_CPP_GUIDELINE_1
using namespace std;
int interpret(string input) {
    Lexer lexer(input);

    vector<Token> tokens = lexer.allTokens();

    // tokens.size() is a long unsigned int, use to prevent -Wall or -Wextra warnings
    cout << "Lexer tokens:" << endl;
    for (long unsigned int i = 0; i < tokens.size(); i++) {
        cout << tokens[i];
    }
    Parser parser(input);
    Block block;
    try {
        block = parser.parseProgram();
    }
    catch (string error) {
        cout << error;
        return 1;
    }
    cout << "Parser blocks:" << endl;
    for (int i = 0; i < block.size(); i++) {
        cout << block[i]->print() << endl;
    }
    Interpreter interpreter(input);
    try {
        interpreter.interpret();
    }
    catch (std::string error) {
        cout << error;
        return 1;
    }

    cout << "Variables: " << endl;
    cout << interpreter.GLOBAL_SCOPE.size() << endl;
    return 0;
}
int main () {
  ifstream cin("test.in");
  std::string input((std::istreambuf_iterator<char>(cin)), (std::istreambuf_iterator<char>()));
  return interpret(input);
}
