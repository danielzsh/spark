#pragma once
#include <string>
#include <vector>
#include "../Lexer/tokens.h"
class AstNode {
  private:
  std::string type;
  public:
  AstNode () {};
  AstNode (std::string typePass) {
    type = typePass;
  }
};
class BinOp : public AstNode {
  private:
  AstNode* left;
  Token op;
  AstNode* right;
  public:
  BinOp () {};
  BinOp (AstNode* leftPass, Token opPass, AstNode* rightPass) {
    left = leftPass;
    op = opPass;
    right = rightPass;
  } 
};
class Num : public AstNode {
  private:
    Token token;
    int value;
  public:
    Num (Token t) {
      token = t;
      value = std::stoi(t.value); // TODO add support for using E/e for exponents
    }
};
class UnOp : public AstNode {
  private:
    Token op;
    AstNode* expr;
  public:
    UnOp (Token opPass, AstNode* exprPass) {
      op = opPass;
      expr = exprPass;
    }
};
class Block : public AstNode {
  private:
    std::vector<AstNode*> children;
  public:
    void append (AstNode* node) {
      children.push_back(node);
    }
};
class Var : public AstNode {
  private:
    Token token;
    std::string value;
  public:
    Var () {}
    Var (Token t) {
      token = t;
      value = t.value;
    }
};
class Assign : public AstNode {
  private:
    Var var;
    Token op;
    AstNode* right;
  public:
    Assign(Var left, Token t, AstNode* r) {
      var = left;
      op = t;
      right = r;
    }
};
class NoOp : public AstNode {

};


