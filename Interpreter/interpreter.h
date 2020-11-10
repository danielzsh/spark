#pragma once
#include "../Parser/parser.h"
#include <map>
#include <string>
#include <cmath>
#include "Symbols.h"
#include <typeinfo>
#include <variant>
#include <cassert>
#include "./Stack.h"
namespace interpreter {
	class Interpreter
	{
	private:
		Parser parser;
		ScopedSymbolTable symTab;
		ScopedSymbolTable& currentSymTab = symTab;
	public:
		CallStack stack;
		Interpreter(std::string input) : parser(input), symTab("global", 1) {

		}
		~Interpreter() {}
		std::string getType(AstNode* node) {
			if (node->print() == "BinOp") {
				BinOp* binOp = static_cast<BinOp*>(node);
				if (binOp->op.type == IntDiv) return "int";
				else return "real";
			}
			else if (node->print() == "Var") {
				Var* var = static_cast<Var*>(node);
				return symTab.lookup(var->value)->type->name;
			}
			else if (node->print() == "Num") {
				Num* num = static_cast<Num*>(node);
				if (num->value == ceil(num->value)) return "int";
				else return "real";
			}
			else if (node->print() == "UnOp") {
				UnOp* unOp = static_cast<UnOp*>(node);
				return getType(unOp->expr);
			}
		}
		template<class T>
		T visit(AstNode* node) {
			if constexpr (std::is_same_v<T, void>) {
				if (node->print() == "Block") visit_Block(*static_cast<Block*>(node));
				else if (node->print() == "Assign") visit_Assign(*static_cast<class Assign*>(node));
				else if (node->print() == "NoOp") visit_NoOp();
				else if (node->print() == "Print") visit_Print(*static_cast<Print*>(node));
				else if (node->print() == "FunctionCall");
				else {
					std::string error = "Error: void operation not recognized";
					throw error;
				}
			}
			else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, double>) {
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
			else if constexpr (std::is_same_v<T, std::string>) {
				return visit_Var<T>(*static_cast<Var*>(node));
			}
			else {
				std::string error("Type not recognized");
				throw error;
			}
		}
		void visit_Print(Print p) {
			cout << visit_String(p.str);
			cout << endl;
		}
		std::string visit_String(String str) {
			int expr_index = 0;
			std::string result = "";
			for (int i = 0; i < str.raw.size(); i++) {
				if (str.raw[i] == '{') {
					AstNode* node = str.expr[expr_index];
					expr_index++;
					if (getType(node) == "int") {
						result += std::to_string(visit<int>(node));
					}
					else if (getType(node) == "real") {
						result += std::to_string(visit<double>(node));
					}
					else if (getType(node) == "string") {
						result += visit<std::string>(node);
					}
					i++;
				}
				else result += str.raw[i];
			}
			return result;
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
			ActivationRecord& ar = stack.peek();
			if (type == "int") {
				ar[var_name] = visit<int>(assign.right);
				return;
			}
			else if (type == "real") {
				ar[var_name] = visit<double>(assign.right);
				return;
			}
			else if (type == "string") {
				ar[var_name] = visit_String(*static_cast<String*>(assign.right));
				return;
			}
			else {
				std::string error("Var type not supported");
				throw error;
			}
		}
		template<class T>
		T visit_Var(Var var) {
			ActivationRecord& ar = stack.peek();
			if (ar.members.find(var.value) != ar.members.end()) {
				if constexpr (std::is_same_v<T, int>) {
					std::string type = symTab.lookup(var.value)->type->name;
					if (type == "int") return (T)std::get<int>(ar[var.value]);
					else {
						std::string error("Wanted integer, got " + type);
						throw error;
					}
				}
				else if constexpr (std::is_same_v<T, double>) {
					std::string type = symTab.lookup(var.value)->type->name;
					if (type == "real") return (T)std::get<double>(ar[var.value]);
					else if (type == "int") {
						return (T)std::get<int>(ar[var.value]);
					}
					else {
						std::string error("Wanted real, got " + type);
						throw error;
					}
				}
				else if constexpr (std::is_same_v<T, std::string>) {
					std::string type = symTab.lookup(var.value)->type->name;
					if (type == "string") return std::get<std::string>(ar[var.value]);
					else {
						std::string error("Wanted string, got " + type);
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
		ScopedSymbolTable getSymTab() {
			return symTab;
		}
		void visit_NoOp() {}
		void interpret() {
			Program p;
			try {
				p = parser.parseProgram();
			}
			catch (std::string error) {
				cout << error << endl;
				return;
			}
			SymbolTableBuilder symtabBuilder("global", 1);
			//std::cout << "Building symtab...\n";
			try {
				symtabBuilder.visit(&p);
			}
			catch (std::string error) {
				cout << error << endl;
				return;
			}
			//std::cout << "Finished building symtab...\n";
			symTab = symtabBuilder.currentScope;
			//std::cout << "Interpreting...\n";
			ARType t = ARType::PROGRAM;
			ActivationRecord ar(t, 1);
			stack.push(ar);
			visit_Block(p.block);
			stack.pop();
		}
	};
}