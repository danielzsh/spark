#pragma once

#include <string>
enum TokenType {
  Identifier = 0,
  Number,
    
  /// Arithmetic operators
  Plus,                  // +
  Minus,                 // -
  Times,                 // *
  Div,                   // /
    
  /// Comparison operators
  GreaterThan,          // >
  GreaterThanOrEqual, // >=
  LessThan,             // <
  LessThanOrEqual,    // <=
  Equal,                 // == 
    
  /// Assignment operator
  Assign,                // = 
 
  /// Parenthesis
  LeftParenthesis,      // (
  RightParenthesis,     // )
  /// Brackets
  LeftBracket,
  RightBracket,
    
  /// Special tokens
  EndOfInput,
  Semicolon
};

class Token {
   
  public:
    TokenType type;
    std::string value;
    int line;
    int column;
    Token() {};
    Token(TokenType typePass, std::string valuePass, int linePass, int columnPass) {
      type = typePass;
      value = valuePass;
      line = linePass;
      column = columnPass;
    }
};
std::ostream& operator << (std::ostream& os, const Token& token) {
  os << token.type << " " << token.value << " " << token.line << " " << token.column << std::endl;
  return os;
}