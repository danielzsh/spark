#pragma once

#include "FSM.h"
#include "tokens.h"
#include "../Utils/CharUtils.h"
#include <vector>
#include <iostream>
#include <map>
#include <algorithm>
using namespace CharUtils;
using namespace std;
class Lexer {
  private:
    string input;
    unsigned int position = 0;
    int line = 0;
    int column = 0;
   
  public:
    map<string, TokenType> keywords = {
        {"main", MAIN},
        {"int", INT},
        {"real", REAL},
        {"vars", VARS},
        {"program", PROGRAM},
        {"def", FUNCTION},
        {"print", PRINT},
        {"string", STRING},
        {"return", RETURN}
    };
    Lexer(string inputPass) {
      input = inputPass;
    }
    int getLine () {
      return line;
    }
    int getCol () {
      return column;
    }
    char getChar() {
        return input[position];
    }
    vector<Token> allTokens () {
      vector<Token> tokens;
      Token currentToken;
      do {
        currentToken = nextToken();
        if (currentToken.type == EndOfInput) {
          break;
        }
        tokens.push_back(currentToken);
      } while (currentToken.type != EndOfInput);
      return tokens;
    }
    std::string SkipWhiteSpaceAndNewlines() {
        std::string whitespace = "";
        while (position < input.length() && isWhitespaceOrNewline(input[position])) {
            if (isNewLine(input[position])) {
                line++;
                column = 0;
                whitespace += "\n";
            }
            else column++;
            position++;
            whitespace += " ";
        }
        return whitespace;
    }
    Token nextToken () {
        SkipWhiteSpaceAndNewlines();
      if (position >= input.length()) {
        Token token(EndOfInput, "", line, column);
        return token;
      }
      if (input[position] == '#') {
          position++;
          column++;
          while (input[position] != '#') {
              position++;
              column++;
          }
          position++;
          column++;
      }
      SkipWhiteSpaceAndNewlines();
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
      if (character == '{') {
        position++;
        column++;
        Token token(LeftBracket, "{", line, column);
        return token;
      }
      if (character == '}') {
        position++;
        column++;
        Token token(RightBracket, "}", line, column);
        return token;
      }
      if (character == ';') {
        position++;
        column++;
        Token token(Semicolon, ";", line, column);
        return token;
      }
      if (character == ':') {
          position++;
          column++;
          Token token(Colon, ":", line, column);
          return token;
      }
      if (character == ',') {
          position++;
          column++;
          Token token(Comma, ",", line, column);
          return token;
      }
      if (character == '\'') {
          position++;
          column++;
          Token token(Apostrophe, "\'", line, column);
          return token;
      }
        std::string error = "Unrecognized: " + input[position];
        throw error;
    }
    Token recognizeParenthesis() {
      char character = input[position];
      position++;
      column++;
      if (character == '(') {
          Token token(LeftParenthesis, "(", line, column);
          return token;
      }
      Token token(RightParenthesis, ")", line, column);
      return token;
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
      Token token;
      switch (character) {
        case '>':
            if (isLookaheadEqualSymbol) {
                token.set(GreaterThanOrEqual, ">=", line, column);
          }
            else token.set(GreaterThan, ">", line, column);
            return token;
          break;
        case '<':
            if (isLookaheadEqualSymbol) {
                token.set(LessThanOrEqual, "<=", line, column);
            }
            else token.set(LessThan, "<", line, column);
            return token;
          break;
        case '=':
            if (isLookaheadEqualSymbol) {
                token.set(Equal, "==", line, column);
            }
            else token.set(Assign, "=", line, column);
            return token;
          break;
        default:
          std::string error = "operation not found";
          throw error;
          break;
      }
    }
    Token recognizeArithmeticOperator () {
      char character = input[position];
      position++;
      column++;
      switch (character) {
      case '+': {
          Token token(Plus, "+", line, column);
          return token;
      }
              break;
      case '-': {
          char lookahead = (position < input.length()) ? input[position] : '\0';
          if (lookahead != '\0' && lookahead == '>') {
              Token token(Arrow, "->", line, column);
              position++;
              column++;
              return token;
          }
          Token token(Minus, "-", line, column);
          return token;
      }
              break;
      case '*': {
          Token token(Times, "*", line, column);
          return token;
      }
          break;
      case '/': {
          char lookahead = (position < input.length()) ? input[position] : '\0';
          if (lookahead != '\0' && lookahead == '/') {
              position++;
              column++;
              Token token(IntDiv, "//", line, column);
              return token;
          }
          else {
              Token token(Div, "/", line, column);
              return token;
          }
          
      }
          break;
        default:
          break;
      }
      std::string error = "operation not found";
      throw error;
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
      if (keywords.count(identifier)) {
          Token token(keywords[identifier], identifier, line, column);
          return token;
      }
      Token token(Identifier, identifier, line, column);
      return token;
    }
    Token recognizeNumber () {
      FSM fsm = buildNumberRecognizer();
      string fsmInput = input.substr(position);
      string fsmOutput = fsm.run(fsmInput);
      position += fsmOutput.length();
      column += fsmOutput.length();
      if (fsm.endState == 4) {
          Token token(Real, fsmOutput, line, column);
          return token;
      }
      Token token(Integer, fsmOutput, line, column);
      return token;
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
      vector<int> states{ Initial, Integer, BeginNumberWithFractionalPart, NumberWithFractionalPart, BeginNumberWithExponent, BeginNumberWithSignedExponent, NumberWithExponent };
      vector<int> acceptingStates{ Integer, NumberWithFractionalPart, NumberWithExponent };
      FSM fsm(states, acceptingStates, 1, [] (int currentState, char character) -> int {
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
