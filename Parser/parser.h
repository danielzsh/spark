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
    void error (TokenType expected, TokenType received) {
        std::string error = "Invalid syntax: Expected " + std::to_string(expected) + ", got " + std::to_string(received);
        throw error;
    } 
    void eat (TokenType type) {
      if (currentToken.type == type) currentToken = lexer.nextToken();
      else error(type, currentToken.type);
    }
    Block parseProgram() {
      eat(MAIN);
      eat(LeftParenthesis);
      eat(RightParenthesis);
      return parseBlock();

    }
    Block parseBlock() {  
      eat(LeftBracket);
      Block block(parseDeclarations());
      eat(PROGRAM);
      eat(Colon);
      AstNode* node = parseStatement();
      block.append(node);
      while (currentToken.type == Semicolon) {
        eat(Semicolon);
        node = parseStatement();
        block.append(node);
      }
      eat(RightBracket);
      return block;
    }
    std::vector<VarDecl> parseDeclarations() {
        std::vector<VarDecl> decl;
        if (currentToken.type == VARS) {
            eat(VARS);
            eat(Colon);
            while (currentToken.type == INT || currentToken.type == REAL)
            {
                std::vector<VarDecl> line = parseVarDeclarations();
                decl.insert(decl.end(), line.begin(), line.end());
            }     
        }
        return decl;
    }
    std::vector<VarDecl> parseVarDeclarations() {
        Type type = parseType();
        eat(Colon);
        std::vector<Var> declarations(1, Var(currentToken));
        eat(Identifier);
        while (currentToken.type == Comma)
        {
            eat(Comma);
            declarations.push_back(Var(currentToken));
            eat(Identifier);
        }
        eat(Semicolon);
        std::vector<VarDecl> var_decl(declarations.size());
        for (unsigned int i = 0; i < var_decl.size(); i++) {
            VarDecl varDecl(type, declarations[i]);
            var_decl[i] = varDecl;
        }
        return var_decl;
    }
    Type parseType() {
        Token token = currentToken;
        if (currentToken.type == INT) eat(INT);
        else eat(REAL);
        Type node = Type(token);
        return node;
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
      else if (token.type == Integer) {
        eat(Integer);
        node = new Num(token);
        return node;
      }
      else if (token.type == Real) {
          eat(Real);
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
