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
        std::string error = "Invalid syntax";
        throw error;
    } 
    void eat (TokenType type) {
      if (currentToken.type == type) currentToken = lexer.nextToken();
      else error();
    }
    Block parseProgram() {
      eat(MAIN);
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
        node = new Block(parseBlock());
        return node;
      }
      if (currentToken.type == Identifier) {
        node = new class Assign(parseAssignment());
        return node;
      }
      else {
        node = new NoOp();
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
      AstNode* node;
      Token token = currentToken;
      if (token.type == Plus) {
        eat(Plus);
        AstNode* factor = parseFactor();
        node = new UnOp(token, factor);
        return node;
      }
      else if (token.type == Minus) {
        eat(Minus);
        AstNode* factor = parseFactor();
        node = new UnOp(token, factor);
        return node;
      }
      else if (token.type == Number) {
        eat(Number);
        node = new Num(token);
        return node;
      }
      else if (token.type == LeftParenthesis) {
        eat(LeftParenthesis);
        AstNode* expr = parseExpression();
        node = expr;
        eat(RightParenthesis);
        return node;
      }
      else {
        node = new Var(parseVariable());
        return node;
      }
    }
};
