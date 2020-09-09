#include <iostream>
#include <fstream>
#include "Lexer.h"
using namespace std;
int main () {
  ifstream fin("test.in");
  std::string input( (std::istreambuf_iterator<char>(fin) ),
                       (std::istreambuf_iterator<char>()    ) );

  Lexer lexer(input);
  vector<Token> tokens = lexer.allTokens();
  for (int i = 0; i < tokens.size(); i++) {
    cout << tokens[i];
  }
}