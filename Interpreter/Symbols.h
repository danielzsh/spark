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
class ProcedureSymbol : public Symbol {
public:
	std::vector<VarSymbol> params;
	ProcedureSymbol(std::string name, std::vector<VarSymbol> p) : Symbol(name), params(p) {
	}
	ProcedureSymbol(std::string name) : Symbol(name) {
	}
};
class ScopedSymbolTable {
private:
	std::string scope_name;
	int scope_level;
public:
	ScopedSymbolTable* enclosingScope = NULL;
	std::map<std::string, Symbol*> symbols;
	ScopedSymbolTable(std::string name, int level) {
		define(new BuiltinTypeSymbol("int"));
		define(new BuiltinTypeSymbol("real"));
		define(new BuiltinTypeSymbol("string"));
		scope_name = name;
		scope_level = level;
	}
	void define(Symbol* symbol) {
		// std::cout << "Define: " << symbol->print() << std::endl;
		symbols[symbol->name] = symbol;
	}
	Symbol* lookup(std::string name) {
		// std::cout << "Lookup: " << name << "\n";
		if (symbols.count(name)) {
			return symbols[name];
		}
		else {
			return new Symbol();
		}
	}
	std::string print() {
		std::string rep("");
		rep += "SCOPE (SCOPED SYMBOL TABLE)\n==================\n";
		rep += "Scope name: " + scope_name + "\n";
		rep += "Scope level: " + std::to_string(scope_level) + "\n";
		rep += "Contents:\n...................\n";
		for (auto pair : symbols) {
			rep += pair.first + ": " + pair.second->print() + "\n";
		}
		rep += "====================\n";
		return rep;
	}
};
class SymbolTableBuilder {
public:
	ScopedSymbolTable symtab;
	ScopedSymbolTable& currentScope;
	SymbolTableBuilder(std::string name, int level) : symtab("global", 1), currentScope(symtab) {
	}
	void visit(AstNode * node) {
		if (node->print() == "Program") visit_Program(*static_cast<Program*>(node));
		else if (node->print() == "ProcedureDecl") visit_ProcedureDecl(*static_cast<ProcedureDecl*>(node));
		else if (node->print() == "Block") visit_Block(*static_cast<Block*>(node));
		else if (node->print() == "UnOp") visit_UnOp(*static_cast<UnOp*>(node));
		else if (node->print() == "Var") visit_Var(*static_cast<Var*>(node));
		else if (node->print() == "BinOp") return visit_BinOp(*static_cast<BinOp*>(node));
		else if (node->print() == "Assign") visit_Assign(*static_cast<class Assign*>(node));
	}
	void visit_Program(Program program) {
		cout << "Enter scope: GLOBAL\n";
		ScopedSymbolTable globalScope("global", 1);
		currentScope = globalScope;
		visit_Block(program.block);
		cout << currentScope.print();
		cout << "Leave scope: GLOBAL\n";
	}
	void visit_ProcedureDecl(ProcedureDecl proc) {
		ProcedureSymbol* proc_symbol = new ProcedureSymbol(proc.name);
		currentScope.define(proc_symbol);
		cout << "Enter scope: " << proc.name << endl;
		ScopedSymbolTable proc_scope(proc.name, 2);
		ScopedSymbolTable* enclosing = new ScopedSymbolTable(currentScope);
		proc_scope.enclosingScope = enclosing;
		currentScope = proc_scope;
		for (VarDecl param : proc.params) {
			BuiltinTypeSymbol* type = static_cast<BuiltinTypeSymbol*>(currentScope.lookup(param.type.type));
			std::string param_name = param.var.value;
			Symbol* var = new VarSymbol(param_name, type);
			currentScope.define(var);
			VarSymbol var_symbol(param_name, type);
			proc_symbol->params.push_back(var_symbol);
		}
		visit_Block(proc.block);
		cout << currentScope.print();
		currentScope = *proc_scope.enclosingScope;
	}
	void visit_Block(Block block) {
		for (VarDecl decl : block.declarations) {
			visit_VarDecl(decl);
		}
		for (ProcedureDecl* decl : block.procedures) {
			visit(decl);
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
		Symbol* var_symbol = currentScope.lookup(var_name);
		if (var_symbol->name == "") {
			std::string error = "Error: variable not defined: " + var_name;
			throw error;
		}
		visit(assign.right);
	}
	void visit_Var(Var var) {
		std::string var_name = var.value;
		Symbol* var_symbol = currentScope.lookup(var_name);
		if (var_symbol->name == "") {
			std::string error = "Error: variable not defined: " + var_name;
			throw error;
		}
	}
	void visit_VarDecl(VarDecl varDecl) {
		std::string type_name = varDecl.type.type;
		BuiltinTypeSymbol* type_symbol = static_cast<BuiltinTypeSymbol*>(currentScope.lookup(type_name));
		std::string var_name = varDecl.var.value;
		if (currentScope.lookup(var_name)->name != "") {
			std::string error("Multiple definition of variable " + var_name);
			throw error;
		}
		VarSymbol* var = new VarSymbol(var_name, type_symbol);
		currentScope.define(var);
	}
};