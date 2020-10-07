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
  public:
  AstNode* left;
  Token op;
  AstNode* right;
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
    double value;
    Num (Token t) {
      token = t;
      if (t.type == Integer) value = std::stoi(t.value); // TODO add support for using E/e for exponents
      else value = std::stod(t.value);
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
class Type : public AstNode {
public:
    Token token;
    string type;
    Type() {}
    Type(Token t) : token(t) {
        type = t.value;
    }
    std::string print() {
        std::string s = "Type";
        return s;
    }
};
class VarDecl : public AstNode {
public:
    Var var;
    Type type;
    VarDecl() {}
    VarDecl(Type t, Var v) : var(v), type(t) {};
    std::string print() {
        std::string s = "VarDecl";
        return s;
    }
};
class ProcedureDecl;
class Block : public AstNode {
public:
    std::vector<AstNode*> children;
    std::vector<VarDecl> declarations;
    std::vector<ProcedureDecl*> procedures;
    Block () {}
    Block(std::vector<VarDecl> d) : declarations(d) {}
    void append(AstNode* node) {
        children.push_back(node);
    }
    AstNode* operator [] (int i) {
        return children[i];
    }
    int size() {
        return children.size();
    }
    std::string print() {
        std::string s = "Block";
        return s;
    }
};
class ProcedureDecl : public AstNode {
public:
    Block block;
    std::string name;
    ProcedureDecl(std::string n, Block b) : block(b) {
        name = n;
    }
    std::string print() {
        return "ProcedureDecl";
    }
};


