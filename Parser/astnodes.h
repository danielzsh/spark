#pragma once
#include <string>
#include <vector>
#include "../Lexer/tokens.h"
#include "../Interpreter/Symbols.h"
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
class String : public AstNode {
public:
    std::string raw;
    std::vector<AstNode*> expr;
    void append(AstNode* node) {
        expr.push_back(node);
    }
    std::string print() {
        return "String";
    }
};
class Boolean : public AstNode {
public:
    bool val;
    std::string print() {
        return "Boolean";
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
class Print : public AstNode {
public:
    String str;
    Token p;
    Print(String s, Token pPass) {
        str = s;
        p = pPass;
    }
    std::string print() {
        std::string s = "Print";
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
    std::string type = "unchanged";
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
class FunctionDecl;

class Block : public AstNode {
public:
    std::vector<AstNode*> children;
    std::vector<VarDecl> declarations;
    std::vector<FunctionDecl*> functions;
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
class If : public AstNode {
public:
    AstNode* b;
    Block body;
    If(AstNode* b, Block body) {
        this->b = b;
        this->body = body;
    }
    std::string print() {
        return "If";
    }
};
class FunctionSymbol : public Symbol {
public:
    std::vector<VarSymbol> params;
    BuiltinTypeSymbol* type = new BuiltinTypeSymbol("void");
    FunctionSymbol(std::string name, std::vector<VarSymbol> p) : Symbol(name), params(p) {
    }
    FunctionSymbol(std::string name) : Symbol(name) {
    }
    FunctionSymbol() {}
    Block blockAst;
    AstNode* ret = NULL;
    bool isVoid = true;
};
class Program : public AstNode {
public:
    Block block;
    Program(Block b) : block(b) {}
    Program() {}
    std::string print() {
        return "Program";
    }
};
class FunctionDecl : public AstNode {
public:
    Block block;
    std::vector<VarDecl> params;
    std::string name;
    Type type;
    AstNode* retStatement;
    FunctionDecl(std::string n, Block b, std::vector<VarDecl> p) : block(b) {
        name = n;
        params = p;
    }
    std::string print() {
        return "FunctionDecl";
    }
    bool isVoid = true;
};
class FunctionCall : public AstNode {
public:
    std::string func_name;
    std::vector<AstNode*> params;
    Token token;
    std::string type;
    FunctionCall(std::string func_name, std::vector<AstNode*> params, Token token) {
        this->func_name = func_name;
        this->params = params;
        this->token = token;
    }
    std::string print() {
        return "FunctionCall";
    }
    FunctionSymbol funcSymbol;
};


