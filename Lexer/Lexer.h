#include "FSM.h"
#include "tokens.h"
#include "../Utils/CharUtils.h"
#include <vector>
#include <iostream>
using namespace CharUtils;
using namespace std;
class Lexer {
  private:
    string input;
    int position = 0;
    int line = 0;
    int column = 0;
  public:
    Lexer(string inputPass) {
      input = inputPass;
    }
    vector<Token> allTokens () {
      vector<Token> tokens;
      while (position < input.length()) {
        tokens.push_back(nextToken());
      }
      return tokens;
    }
    Token nextToken () {
      while (position < input.length() && isWhitespaceOrNewline(input[position])) {
        if (isNewLine(input[position])) {
          line++;
          column = 0;
        }
        else column++;
        position++;
      }
      if (position >= input.length()) {
        return *(new Token(EndOfInput, "", line, column));
      }
      char character = input[position];
      if (isLetter(character)) return recognizeIdentifier();
      if (isDigit(character)) return recognizeNumber();
      if (isOperator(character)) {
        if (isComparisonOperator(character)) {
          return recognizeComparisonOperator();
        }
        if (isArithmeticOperator(character)) {
          return recognizeArithmeticOperator();
        }
      }
      if (isParenthesis(character)) {
        return recognizeParenthesis();
      }
      throw "false";
    }
    Token recognizeParenthesis() {
      char character = input[position];
      position++;
      column++;
      if (character == '(') return *(new Token(LeftParenthesis, "(", line, column));
      return *(new Token(RightParenthesis, ")", line, column));
    }
    Token recognizeComparisonOperator() {
      char character = input[position];
      char lookahead = (position + 1 < input.length()) ? input[position + 1] : '\0';
      bool isLookaheadEqualSymbol = (lookahead != '\0' && lookahead == '=');
      position += 1;
      column += 1;
      if (isLookaheadEqualSymbol) {
        position++;
        column++;
      }
      switch (character) {
        case '>':
          return (isLookaheadEqualSymbol) ? *(new Token(GreaterThanOrEqual, ">=", line, column)) : *(new Token(GreaterThan, ">", line, column));
          break;
        case '<':
          return (isLookaheadEqualSymbol) ? *(new Token(LessThanOrEqual, "<=", line, column)) : *(new Token(LessThan, "<", line, column));
          break;
        case '=':
          return (isLookaheadEqualSymbol) ? *(new Token(Equal, "==", line, column)) : *(new Token(Assign, "=", line, column));
          break;
        default:
          break;
      }
      try {throw "operation not found";}
      catch (string error) {
        cout << "error: " << error;
      }
      throw "error: invalid";
    }
    Token recognizeArithmeticOperator () {
      char character = input[position];
      position++;
      column++;
      switch (character) {
        case '+':
          return *(new Token(Plus, "+", line, column));
          break;
        case '-':
          return *(new Token(Minus, "-", line, column));
          break;
        case '*':
          return *(new Token(Times, "*", line, column));
          break;
        case '/':
          return *(new Token(Div, "/", line, column));
          break;
        default:
          break;
      }
      try {throw "operation not found";}
      catch (string error) {
        cout << "error: " << error;
      }
      return *(new Token(Div, "/", line, column));
    }
    Token recognizeIdentifier() {
      string identifier = "";
      while (position < input.length()) {
        char character = input[position];
        if (!(isLetter(character) || isDigit(character) || character == '_')) {
          break;
        }
        identifier += character;
        position++;
        column++;
      }
      return *(new Token(Identifier, identifier, line, column));
    }
    Token recognizeNumber () {
      FSM fsm = buildNumberRecognizer();
      string fsmInput = input.substr(position);
      string fsmOutput = fsm.run(fsmInput);
      position += fsmOutput.length();
      column += fsmOutput.length();
      return *(new Token(Number,fsmOutput, line, column));
    }
    FSM buildNumberRecognizer () {
      enum State {
        Initial = 1,
        Integer,
        BeginNumberWithFractionalPart,
        NumberWithFractionalPart,
        BeginNumberWithExponent,
        BeginNumberWithSignedExponent,
        NumberWithExponent,
        NoNextState = 0
      };
      FSM fsm(*(new vector<int>{Initial, Integer, BeginNumberWithFractionalPart, NumberWithFractionalPart, BeginNumberWithExponent, BeginNumberWithSignedExponent, NumberWithExponent}), *(new vector<int>{Integer, NumberWithFractionalPart, NumberWithExponent}), 1, [] (int currentState, char character) -> int {
        switch (currentState) {
          case Initial:
            if (isDigit(character)) {
              return Integer;
            }
            break;
          case Integer:
            if (isDigit(character)) {
              return Integer;
            }
            if (character == '.') {
              return BeginNumberWithFractionalPart;
            }
            if (character == 'E' || character == 'e') {
              return BeginNumberWithExponent;
            }
            break;
          case BeginNumberWithFractionalPart: 
            if (isDigit(character)) return NumberWithFractionalPart;
            break;
          case NumberWithFractionalPart:
            if (isDigit(character)) return NumberWithFractionalPart;
            if (character == 'E' || character == 'e') {
              return BeginNumberWithExponent;
            }
            break;
          case BeginNumberWithExponent:
            if (character == '+' || character == '-') {
              return BeginNumberWithSignedExponent;
            }
            if (isDigit(character)) return NumberWithExponent;
            break;
          case BeginNumberWithSignedExponent:
            if (isDigit(character)) return NumberWithExponent;
            break;
          case NumberWithExponent:
            if (isDigit(character)) return NumberWithExponent;
            break;
          default:
            break;
        }
        return NoNextState;
      });
      return fsm;
    }
};
