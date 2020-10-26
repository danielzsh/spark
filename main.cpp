#include <iostream>
#include <fstream>
#include "Parser/parser.h"
#include "Lexer/Lexer.h"
#include "Interpreter/interpreter.h"
#include "Interpreter/Symbols.h"

// Tip: Don't use using namespace, see https://bit.ly/aaron_help_CPP_GUIDELINE_1
using namespace std;
using namespace interpreter;
std::string readFileToVector(const std::string& filename)
{
    std::ifstream source;
    source.open(filename);
    std::string lines;
    std::string line;
    while (std::getline(source, line))
    {
        lines += line + '\n';
    }
    return lines;
}
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
        cout << error << "\n";
        return 1;
    }
    cout << "Parser blocks:" << endl;
    for (int i = 0; i < block.size(); i++) {
        cout << block[i]->print() << endl;
    }
    Interpreter interpreter(input);
    cout << "Interpreting...\n";
    try {
        interpreter.interpret();
    }
    catch (std::string error) {
        cout << error << "\n";
        return 1;
    }

    cout << "Variables: " << endl;
    ScopedSymbolTable symtab = interpreter.getSymTab();
    cout << symtab.print();
    for (auto const& pair : interpreter.GLOBAL_SCOPE) {
        cout << pair.first << " ";
        std::string type = symtab.lookup(pair.first)->type->name;
        if (type == "int") {
            cout << std::get<int>(pair.second);
        }
        else if (type == "real") {
            cout << std::get<double>(pair.second);
        }
        else if (type == "string") {
            cout << std::get<std::string>(pair.second);
        }
        cout << endl;
    }
    return 0;
}
int main (int argc, char** argv) {
  std::string input(readFileToVector(argv[1]));
  Interpreter interpreter(input);
  interpreter.interpret();
  return 0;
}
