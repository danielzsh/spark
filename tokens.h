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
    
  /// Special tokens
  EndOfInput
};

class Token {
   
  public:
    TokenType type;
    string value;
    int line;
    int column;
    Token(TokenType typePass, string valuePass, int linePass, int columnPass) {
      type = typePass;
      value = valuePass;
      line = linePass;
      column = columnPass;
    }
};
ostream& operator << (ostream& os, const Token& token) {
  os << token.type << " " << token.value << " " << token.line << " " << token.column << endl;
  return os;
}