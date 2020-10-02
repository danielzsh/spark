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
  virtual std::string print () = 0;
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
  Token getOp () {
    return op;
  }
  std::string print () {
    std::string s = "BinOp";
    return s;
  } 
};
class Num : public AstNode {
  private:
    Token token;
  public:
    int value;
    Num (Token t) {
      token = t;
      value = std::stoi(t.value); // TODO add support for using E/e for exponents
    }
    std::string print () {
    std::string s = "Num";
    return s;
  } 
};
class UnOp : public AstNode {
  public:
    AstNode* expr;
    Token op;
    UnOp (Token opPass, AstNode* exprPass) {
      op = opPass;
      expr = exprPass;
    }
    std::string print () {
    std::string s = "UnOp";
    return s;
  } 
};
class Block : public AstNode {
  public:
    std::vector<AstNode*> children;
    void append (AstNode* node) {
      children.push_back(node);
    }
    AstNode* operator [] (int i) {
      return children[i];
    }
    int size () {
      return children.size();
    }
    std::string print () {
    std::string s = "Block";
    return s;
  } 
};
class Var : public AstNode {
  public:
    Token token;
    std::string value;
    Var () {}
    Var (Token t) {
      token = t;
      value = t.value;
    }
    std::string print () {
    std::string s = "Var";
    return s;
  } 
};
class Assign : public AstNode {
  public:
    Var var;
    Token op;
    AstNode* right;
    Assign(Var left, Token t, AstNode* r) {
      var = left;
      op = t;
      right = r;
    }
    std::string print () {
    std::string s = "Assign";
    return s;
    }
    Token getOp () {
      return op;
    } 
};
class NoOp : public AstNode {
  std::string print () {
    std::string s = "NoOp";
    return s;
  } 
};


