#pragma once
#include "../Lexer/Lexer.h"
#include "astnodes.h"
class Parser {
  private:
    Lexer lexer;
    Token currentToken;
  public:
    Parser (std::string input) : lexer(input) {
      currentToken = lexer.nextToken();
    }
    void error () {
      try {
        std::string error = "Invalid syntax";
        throw error;
      } catch (std::string error) {
        std::cout << error;
      }
    } 
    void eat (TokenType type) {
      if (currentToken.type == type) currentToken = lexer.nextToken();
      else error();
    }
    void eat (TokenType type, string value) {
      if (currentToken.type == type && currentToken.value == value) currentToken = lexer.nextToken();
      else error();
    }
    Block parseProgram() {
      eat(Identifier, "main");
      eat(LeftParenthesis);
      eat(RightParenthesis);
      return parseBlock();

    }
    Block parseBlock() {
      Block block;
      eat(LeftBracket);
      AstNode* node = parseStatement();
      block.append(node);
      while (currentToken.type == Semicolon) {
        eat(Semicolon);
        node = parseStatement();
        block.append(node);
      }
      if (currentToken.type == Identifier) error();
      eat(RightBracket);
      return block;
    }
    AstNode* parseStatement() {
      AstNode* node;
      if (currentToken.type == LeftBracket) {
        static Block block = parseBlock();
        node = &block;
        return node;
      }
      if (currentToken.type == Identifier) {
        static class Assign assign = parseAssignment();
        node = &assign;
        return node;
      }
      else {
        static NoOp noOp;
        node = &noOp;
        return node;
      }
    }
    class Assign parseAssignment() {
      Var var = parseVariable();
      Token token = currentToken;
      eat(Assign);
      AstNode* r = parseExpression();
      class Assign node(var, token, r);
      return node;
    }
    Var parseVariable() {
      Var node(currentToken);
      eat(Identifier);
      return node; 
    }
    AstNode* parseExpression() {
      AstNode* node = parseTerm();
      while (currentToken.type == Plus || currentToken.type == Minus) {
        Token token = currentToken;
        if (token.type == Plus) eat(Plus);
        else if (token.type == Minus) eat(Minus);
        node = new BinOp(node, token, parseTerm());
      }
      return node;
    }
    AstNode* parseTerm() {
      AstNode* node = parseFactor();
      while (currentToken.type == Times || currentToken.type == Div) {
        Token token = currentToken;
        if (token.type == Times) eat(Times);
        else if (token.type == Div) eat(Div);
        node = new BinOp(node, token, parseFactor());
      }
      
      return node;
    }
    AstNode* parseFactor() {
      static AstNode* node;
      Token token = currentToken;
      if (token.type == Plus) {
        eat(Plus);
        static AstNode* factor = parseFactor();
        static UnOp unOp(token, factor);
        node = &unOp;
        return node;
      }
      else if (token.type == Minus) {
        eat(Minus);
        static AstNode* factor = parseFactor();
        static UnOp unOp(token, factor);
        node = &unOp;
        return node;
      }
      else if (token.type == Number) {
        eat(Number);
        static Num factor(token);
        node = &factor;
        return node;
      }
      else if (token.type == LeftParenthesis) {
        eat(LeftParenthesis);
        static AstNode* expr = parseExpression();
        node = expr;
        eat(RightParenthesis);
        return node;
      }
      else {
        Var var = parseVariable();
        node = &var;
        return node;
      }
    }
};