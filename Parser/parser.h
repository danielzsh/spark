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
        std::string error = "Invalid syntax: Expected " + std::to_string(expected) + ", got " + std::to_string(received) + " " + currentToken.value;
        throw error;
    } 
    void eat (TokenType type) {
      if (currentToken.type == type) currentToken = lexer.nextToken();
      else error(type, currentToken.type);
    }
    Program parseProgram() {
      eat(MAIN);
      eat(LeftParenthesis);
      eat(RightParenthesis);
      Program p(parseBlock());
      return p;
    }
    Block parseBlock() {  
      eat(LeftBracket);
      Block block(parseDeclarations());
      block.procedures = parseFunctions();
      eat(PROGRAM);
      eat(Colon);
      AstNode* node = parseStatement();
      block.append(node);
      while (currentToken.type == Semicolon || node->print() == "Block") {
        if (currentToken.type == Semicolon) eat(Semicolon);
        node = parseStatement();
        block.append(node);
      }
      eat(RightBracket);
      return block;
    }
    String parseString() {
        String str;
        str.raw = "";
        eat(Apostrophe);
        while (currentToken.type != Apostrophe) {
            str.raw += currentToken.value;
            if (currentToken.type == LeftBracket) {
                eat(LeftBracket);
                str.append(parseExpression());
                str.raw += currentToken.value;
                std::string whitespace = lexer.SkipWhiteSpaceAndNewlines();
                eat(RightBracket);
                str.raw += whitespace;
            }
            else {
                str.raw += lexer.SkipWhiteSpaceAndNewlines();
                eat(currentToken.type);
            }
        }
        eat(Apostrophe);
        return str;
    }
    std::vector<VarDecl> parseDeclarations() {
        std::vector<VarDecl> decl;
        if (currentToken.type == VARS) {
            eat(VARS);
            eat(Colon);
            while (currentToken.type == INT || currentToken.type == REAL || currentToken.type == STRING)
            {
                std::vector<VarDecl> line = parseVarDeclarations();
                eat(Semicolon);
                decl.insert(decl.end(), line.begin(), line.end());
            }     
        }
       
        return decl;
    }
    std::vector<VarDecl> parseFormalParameters() {
        std::vector<VarDecl> decl;
        while (currentToken.type == INT || currentToken.type == REAL || currentToken.type == STRING)
            {
                std::vector<VarDecl> line = parseVarDeclarations();
                decl.insert(decl.end(), line.begin(), line.end());
                if (currentToken.type != Semicolon) break;
                else eat(Semicolon);
            }

        return decl;
    }
    std::vector<FunctionDecl*> parseFunctions() {
        std::vector<FunctionDecl*> declarations;
        while (currentToken.type == FUNCTION) {
            eat(FUNCTION);
            std::string proc_name = currentToken.value;
            eat(Identifier);
            eat(LeftParenthesis);
            std::vector<VarDecl> params = parseFormalParameters();
            eat(RightParenthesis);
            Block block = parseBlock();
            FunctionDecl* proc_decl = new FunctionDecl(proc_name,  block, params);
            declarations.push_back(proc_decl);
        }
        return declarations;
    }
    FunctionCall parseFunctionCall() {
        Token token = currentToken;
        std::string proc_name = token.value;
        eat(Identifier);
        eat(LeftParenthesis);
        vector<AstNode*> params;
        if (currentToken.type != RightParenthesis) {
            params.push_back(parseExpression());
        }
        while (currentToken.type == Comma) {
            eat(Comma);
            params.push_back(parseExpression());
        }
        eat(RightParenthesis);
        FunctionCall procCall(proc_name, params, token);
        return procCall;
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
        else if (currentToken.type == REAL) eat(REAL);
        else eat(STRING);
        Type node = Type(token);
        return node;
    }
    AstNode* parseStatement() {
      AstNode* node;
      if (currentToken.type == LeftBracket) {
        node = new Block(parseBlock());
        return node;
      }
      if (currentToken.type == PRINT) {
          node = new Print(parsePrint());
          return node;
      }
      if (currentToken.type == Identifier) {
        if (lexer.getChar() == '(') node = new FunctionCall(parseFunctionCall());
        else node = new class Assign(parseAssignment());
        return node;
      }
      else {
        node = new NoOp();
        return node;
      }
    }
    Print parsePrint() {
        Token token = currentToken;
        eat(PRINT);
        eat(LeftParenthesis);
        Print print(parseString(), token);
        eat(RightParenthesis);
        return print;
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
      AstNode* node;
      if (currentToken.type == Apostrophe) {
          node = new String(parseString());
          return node;
      }
      node = parseTerm();
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
      while (currentToken.type == Times || currentToken.type == Div || currentToken.type == IntDiv) {
        Token token = currentToken;
        if (token.type == Times) eat(Times);
        else if (token.type == Div) eat(Div);
        else if (token.type == IntDiv) eat(IntDiv);
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
