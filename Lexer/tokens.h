#pragma once

#include <string>
enum TokenType {
  Identifier = 0,
  Integer,
  Real,
    
  /// Arithmetic operators
  Plus,                  // +
  Minus,                 // -
  Times,                 // *
  Div,                   // /
  IntDiv, // //
    
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
  Apostrophe,  
  /// Special tokens
  EndOfInput,
  Semicolon,
  // Keywords
    MAIN,
    INT,
    REAL,
    STRING,
    VARS,
    PROGRAM,
    FUNCTION,
    PRINT,
    // Miscellaneous
    Colon,
    Comma
};

class Token {
   
  public:
    TokenType type = EndOfInput;
    std::string value = "";
    int line = -1;
    int column = -1;
    Token() {
    }
    Token(TokenType typePass, std::string valuePass, int linePass, int columnPass) {
      type = typePass;
      value = valuePass;
      line = linePass;
      column = columnPass;
    }
    void set(TokenType typePass, std::string valuePass, int linePass, int columnPass) {
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