#pragma once
#include "../Parser/parser.h"
#include <map>
#include <string>
#include <cmath>
#include "Symbols.h"
#include <typeinfo>
class Interpreter
{
private:
	Parser parser;
	SymbolTable symTab;
public:
	union val
	{
		int i;
		double d;
	};
	std::map<std::string,val> GLOBAL_SCOPE;
	Interpreter(std::string input) : parser(input) {
		
	}
	~Interpreter() {}
	template<class T>
	T visit(AstNode* node) {
		if (typeid(T) == typeid(void)) {
			if (node->print() == "Block") visit_Block(*static_cast<Block*>(node));
			else if (node->print() == "Assign") visit_Assign(*static_cast<class Assign*>(node));
			else if (node->print() == "NoOp") visit_NoOp();
			else {
				std::string error = "Error: void operation not recognized";
				throw error;
			}
		}
		else if (typeid(T) == typeid(int) || typeid(T) == typeid(double)) {
			if (node->print() == "BinOp") {
				BinOp binOp = *static_cast<BinOp*>(node);
				return visit_BinOp<T>(binOp);
			}
			else if (node->print() == "Num") {
				return visit_Num<T>(*static_cast<Num*>(node));
			}
			else if (node->print() == "UnOp") {
				return visit_UnOp<T>(*static_cast<UnOp*>(node));
			}
			else if (node->print() == "Var") {
				return visit_Var<T>(*static_cast<Var*>(node));
			}
			else {
				std::string error = "Error: not recognized";
				throw error;
			}
		}
		else {
			std::string error("Type not recognized");
			throw error;
		}
	}
	template <class T>
	T visit_BinOp(BinOp binOp) {
		if (typeid(T) == typeid(int))
		{
			if (binOp.op.type == Div) {
				std::string error = "Error: Float division and int are incompatible types";
				throw error;
			}
			else if (binOp.op.type == Plus) {
				return (T)(visit<int>(binOp.left) + visit<int>(binOp.right));
			}
			else if (binOp.op.type == Minus) {
				return (T)(visit<int>(binOp.left) - visit<int>(binOp.right));
			}
			else if (binOp.op.type == Times) return (T)(visit<int>(binOp.left) * visit<int>(binOp.right));
			else if (binOp.op.type == IntDiv) return (T)(visit<int>(binOp.left) / visit<int>(binOp.right));
			else {
				std::string error("Error: BinOp operation not recognized.");
				throw error;
			}
		}
		else if (typeid(T) == typeid(double)) {
			if (binOp.op.type == Plus) {
				return (T)(visit<double>(binOp.left) + visit<double>(binOp.right));
			}
			else if (binOp.op.type == Minus) {
				return (T)(visit<int>(binOp.left) - visit<int>(binOp.right));
			}
			else if (binOp.op.type == Times) return (T)(visit<double>(binOp.left) * visit<double>(binOp.right));
			else if (binOp.op.type == IntDiv) return (T)(visit<int>(binOp.left) / visit<int>(binOp.right));
			else if (binOp.op.type == Div) {
				return (T)(visit<double>(binOp.left) / visit<double>(binOp.right));
			}
			else {
				std::string error("Error: BinOp operation not recognized.");
				throw error;
			}
		}
		else {
			std::string error("Type not recognized");
			throw error;
		}
	}
	template<class T>
	T visit_Num(Num num) {
		if (typeid(T) == typeid(void)) {
			return (T)(1);
		}
		if (typeid(T) == typeid(int)) {
			if (ceil(num.value) == num.value) return (T)num.value;
			else {
				std::string error = "Error: Wanted int but got double";
				throw error;
			}
		}
		else return (T)num.value;
			
	}
	template <class T>
	T visit_UnOp(UnOp unOp) {
		if (typeid(T) == typeid(void)) {
			std::string error = "void unOp? srsly";
			throw error;
		}
		TokenType op = unOp.op.type;
		if (typeid(T) == typeid(int)) {
			if (op == Plus) return (T)(visit<int>(unOp.expr));
			else return (T)(0 - visit<int>(unOp.expr));
		}
		if (typeid(T) == typeid(double)) {
			if (op == Plus) return (T)(visit<double>(unOp.expr));
			else return (T)(0 - visit<double>(unOp.expr));
		}
		else {
			std::string error("Type not recognized");
			throw error;
		}
	}
	void visit_Block(Block block) {
		for (AstNode* child : block.children) {
			visit<void>(child);
		}
	}
	void visit_Assign(class Assign assign) {
		std::string var_name = assign.var.value;
		std::string type = symTab.lookup(var_name)->type->name;
		cout << type << endl;
		if (type == "int") {
			GLOBAL_SCOPE[var_name].i = visit<int>(assign.right);
			return;
		}
		else if (type == "real") {
			GLOBAL_SCOPE[var_name].d = visit<double>(assign.right);
			return;
		}
		else {
			std::string error("Var type not supported");
			throw error;
		}
	}
	template<class T>
	T visit_Var(Var var) {
		if (GLOBAL_SCOPE.find(var.value) != GLOBAL_SCOPE.end()) {
			if (typeid(T) == typeid(int)) {
				std::string type = symTab.lookup(var.value)->type->name;
				if (type == "int") return (T)GLOBAL_SCOPE[var.value].i;
				else {
					std::string error("Wanted integer, got " + type);
					throw error;
				}
			}
			else if (typeid(T) == typeid(double)) {
				std::string type = symTab.lookup(var.value)->type->name;
				if (type == "real") return (T)GLOBAL_SCOPE[var.value].d;
				else {
					std::string error("Wanted real, got " + type);
					throw error;
				}
			}
			else {
				std::string error("Var type invalid");
				throw error;
			}
		}
		else return (T)0;
	}
	SymbolTable getSymTab() {
		return symTab;
	}
	void visit_NoOp() {}
	void interpret() {
		Block block = parser.parseProgram();
		SymbolTableBuilder symtabBuilder;
		symtabBuilder.visit(&block);
		std::cout << "Finished building symtab...\n";
		symTab = symtabBuilder.symtab;
		visit_Block(block);
	}
};