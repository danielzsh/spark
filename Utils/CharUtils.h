namespace CharUtils {
  bool isLetter (char query) {
    if ((query >= 'a' && query <= 'z') || (query >= 'A' && query <= 'Z')) return true;
    return false;
  }
  bool isDigit (char query) {
    if (query >= '0' && query <= '9') return true;
    return false;
  }
}
bool isParenthesis (char query) {
  if (query == '(' || query == ')') return true;
  return false;
}
bool isOperator (char query) {
  if (query == '+' || query == '-' || query == '*' || query == '/' || query == '>' || query == '<' || query == '=') return true;
  return false;
}
bool isArithmeticOperator (char query) {
  if (query == '+' || query == '-' || query == '*' || query == '/') return true;
  return false;
}
bool isComparisonOperator (char query) {
  if (query == '>' || query == '<' || query == '=') return true;
  return false;
}
bool isWhitespaceOrNewline (char query) {
  if (query == ' ' || query == '\t' || query == '\n') return true;
  return false;
}
bool isNewLine (char query) {
  return (query == '\n');
}