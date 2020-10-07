#pragma once
#include <string>
#include <iostream>
#include <map>
#include "../Parser/astnodes.h"
class Symbol
{
private:
public:
	Symbol(std::string n = "", Symbol* t = NULL) {
		name = n;
		type = t;
	}
	std::string name;
	Symbol* type;
	std::string print() {
		if (type == NULL) return name;
		return "<" + name + ":" + type->print() + ">";
	}
};
class BuiltinTypeSymbol : public Symbol {
public:
	BuiltinTypeSymbol(std::string n) : Symbol(n) {

	}
};
class VarSymbol : public Symbol
{
public:
	VarSymbol(std::string n, BuiltinTypeSymbol* t) : Symbol(n, t) {

	}

private:

};
class SymbolTable {
public:
	std::map<std::string, Symbol*> symbols;
	SymbolTable() {
		define(new BuiltinTypeSymbol("int"));
		define(new BuiltinTypeSymbol("real"));
	}
	void define(Symbol* symbol) {
		std::cout << "Define: " << symbol->print() << std::endl;
		symbols[symbol->name] = symbol;
	}
	Symbol* lookup(std::string name) {
		std::cout << "Lookup: " << name << "\n";
		if (symbols.count(name)) {
			return symbols[name];
		}
		else {
			return new Symbol();
		}
	}
};
class SymbolTableBuilder {
public:
	SymbolTable symtab;
	void visit(AstNode * node) {
		if (node->print() == "Block") visit_Block(*static_cast<Block*>(node));
		else if (node->print() == "UnOp") visit_UnOp(*static_cast<UnOp*>(node));
		else if (node->print() == "Var") visit_Var(*static_cast<Var*>(node));
		else if (node->print() == "BinOp") return visit_BinOp(*static_cast<BinOp*>(node));
		else if (node->print() == "Assign") visit_Assign(*static_cast<class Assign*>(node));
	}
	void visit_Block(Block block) {
		for (VarDecl decl : block.declarations) {
			visit_VarDecl(decl);
		}
		for (AstNode* node : block.children) {
			visit(node);
		}
	}
	void visit_UnOp(UnOp unOp) {
		visit(unOp.expr);
	}
	void visit_BinOp(BinOp binOp) {
		visit(binOp.left);
		visit(binOp.right);
	}
	void visit_Assign(class Assign assign) {
		std::string var_name = assign.var.value;
		Symbol* var_symbol = symtab.lookup(var_name);
		if (var_symbol->name == "") {
			std::string error = "Error: variable not defined: " + var_name;
			throw error;
		}
		visit(assign.right);
	}
	void visit_Var(Var var) {
		std::string var_name = var.value;
		Symbol* var_symbol = symtab.lookup(var_name);
		if (var_symbol->name == "") {
			std::string error = "Error: variable not defined: " + var_name;
			throw error;
		}
	}
	void visit_VarDecl(VarDecl varDecl) {
		std::string type_name = varDecl.type.type;
		BuiltinTypeSymbol* type_symbol = static_cast<BuiltinTypeSymbol*>(symtab.lookup(type_name));
		std::string var_name = varDecl.var.value;
		VarSymbol* var = new VarSymbol(var_name, type_symbol);
		symtab.define(var);
	}
};