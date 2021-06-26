#pragma once
#include <string>
#include <iostream>
#include <map>
#include "../Parser/astnodes.h"
#include "./Symbols.h"
class ScopedSymbolTable {
private:
	std::string scope_name;
public:
	int scope_level;
	ScopedSymbolTable* enclosingScope = NULL;
	std::map<std::string, Symbol*> symbols;
	ScopedSymbolTable(std::string name, int level) {
		define(new BuiltinTypeSymbol("int"));
		define(new BuiltinTypeSymbol("real"));
		define(new BuiltinTypeSymbol("string"));
		define(new BuiltinTypeSymbol("void"));
		define(new BuiltinTypeSymbol("bool"));
		scope_name = name;
		scope_level = level;
	}
	void define(Symbol* symbol) {
		// std::// cout << "Define: " << symbol->print() << std::endl;
		if (lookup(symbol->name, true)->name != "") {
			std::string error("Multiple definition of symbol");
			throw error;
		}
		symbols[symbol->name] = symbol;
	}
	Symbol* lookup(std::string name, bool currentScopeOnly = false) {
		// std::// cout << "Lookup: " << name << "\n";
		if (symbols.count(name)) {
			return symbols[name];
		}
		else {
			if (enclosingScope != NULL && !currentScopeOnly) return enclosingScope->lookup(name);
			else return new Symbol();
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
class SematicAnalyzer {
public:
	ScopedSymbolTable symtab;
	ScopedSymbolTable& currentScope;
	SematicAnalyzer(std::string name, int level) : symtab("global", 1), currentScope(symtab) {
	}
	void visit(AstNode* node) {
		if (node == NULL) return;
		if (node->print() == "Program") visit_Program(*static_cast<Program*>(node));
		else if (node->print() == "FunctionDecl") visit_FunctionDecl(*static_cast<FunctionDecl*>(node));
		else if (node->print() == "FunctionCall") visit_FunctionCall(*static_cast<FunctionCall*>(node));
		else if (node->print() == "Block") visit_Block(*static_cast<Block*>(node));
		else if (node->print() == "UnOp") visit_UnOp(*static_cast<UnOp*>(node));
		else if (node->print() == "Var") visit_Var(*static_cast<Var*>(node));
		else if (node->print() == "BinOp") visit_BinOp(*static_cast<BinOp*>(node));
		else if (node->print() == "Print") {
			Print* p = static_cast<Print*>(node);
			visit(&p->str);
		}
		else if (node->print() == "String") visit_String(*static_cast<String*>(node));
		else if (node->print() == "Assign") visit_Assign(*static_cast<class Assign*>(node));
	}
	void visit_String(String str) {
		for (AstNode* expr : str.expr) visit(expr);
	}
	void visit_FunctionCall(FunctionCall& functionCall) {
		if (functionCall.params.size() != static_cast<FunctionSymbol*>(symtab.lookup(functionCall.func_name))->params.size()) {
			std::string error("Function call parameters do not match function parameters.");
			throw error;
		}
		for (AstNode* param : functionCall.params) {
			visit(param);
		}
		FunctionSymbol funcSymbol = *static_cast<FunctionSymbol*>(symtab.lookup(functionCall.func_name));
		functionCall.funcSymbol = funcSymbol;
		functionCall.type = funcSymbol.type->name;
	}
	void visit_Program(Program& program) {
		// // cout << "Enter scope: GLOBAL\n";
		ScopedSymbolTable globalScope("global", 1);
		currentScope = globalScope;
		visit_Block(program.block);
		// // cout << currentScope.print();
		// // cout << "Leave scope: GLOBAL\n";
	}
	void visit_FunctionDecl(FunctionDecl& func) {
		FunctionSymbol* func_symbol = new FunctionSymbol(func.name);
		BuiltinTypeSymbol* type = new BuiltinTypeSymbol(func.type.type);
		func_symbol->type = type;
		func_symbol->ret = func.retStatement;
		func_symbol->isVoid = func.isVoid;
		currentScope.define(func_symbol);
		// // cout << "Enter scope: " << func.name << endl;
		ScopedSymbolTable func_scope(func.name, currentScope.scope_level + 1);
		ScopedSymbolTable* enclosing = new ScopedSymbolTable(currentScope);
		func_scope.enclosingScope = enclosing;
		currentScope = func_scope;
		for (VarDecl param : func.params) {
			BuiltinTypeSymbol* type = static_cast<BuiltinTypeSymbol*>(currentScope.lookup(param.type.type));
			std::string param_name = param.var.value;
			Symbol* var = new VarSymbol(param_name, type);
			currentScope.define(var);
			VarSymbol var_symbol(param_name, type);
			func_symbol->params.push_back(var_symbol);
		}
		visit_Block(func.block);
		// cout << currentScope.print();
		if (!func_symbol->isVoid) visit(func_symbol->ret);
		currentScope = *func_scope.enclosingScope;
		func_symbol->blockAst = *new Block(func.block);
	}
	void visit_Block(Block block) {
		for (VarDecl decl : block.declarations) {
			visit_VarDecl(decl);
		}
		for (FunctionDecl* decl : block.functions) {
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
	void visit_Assign(class Assign& assign) {
		std::string var_name = assign.var.value;
		Symbol* var_symbol = currentScope.lookup(var_name);
		if (var_symbol->name == "") {
			std::string error = "Error: variable not defined: " + var_name;
			throw error;
		}
		visit(&assign.var);
		visit(assign.right);
	}
	void visit_Var(Var& var) {
		std::string var_name = var.value;
		Symbol* var_symbol = currentScope.lookup(var_name);
		if (var_symbol->name == "") {
			std::string error = "Error: variable not defined: " + var_name;
			throw error;
		}
		var.type = var_symbol->type->name;
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