#include <iostream>
#include <fstream>
#include "Parser/parser.h"
#include "Lexer/Lexer.h"

// Tip: Don't use using namespace, see https://bit.ly/aaron_help_CPP_GUIDELINE_1
using namespace std;

int main () {
  ifstream cin("test.in");
  std::string input((std::istreambuf_iterator<char>(cin)), (std::istreambuf_iterator<char>()));

  Lexer lexer(input);

  vector<Token> tokens = lexer.allTokens();

  // tokens.size() is a long unsigned int, use to prevent -Wall or -Wextra warnings
  for (long unsigned int i = 0; i < tokens.size(); i++) {
    cout << tokens[i];
  }
  Parser parser(input);
  parser.parseProgram();
}