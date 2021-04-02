#pragma once
#include "../Parser/parser.h"
#include <map>
#include <string>
#include <cmath>
#include "../Parser/astnodes.h"
#include "Symbols.h"
#include <typeinfo>
#include <variant>
#include <cassert>
#include "./Stack.h"
#include "./SymbolTable.h"
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
				// cout << "Getting type of Var..." << endl;
				Var* var = dynamic_cast<Var*>(node);
				// cout << var->value << endl;
				// cout << types[var->value];
				const ActivationRecord& ar = stack.peek();
				// cout << ar.name << endl << var->value << endl << var->type << endl;
				return var->type;
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
			else if (node->print() == "FunctionCall") {
				FunctionCall* funcCall = static_cast<FunctionCall*>(node);
				// cout << funcCall->type << endl;
				return funcCall->type;
			}
			else if (node->print() == "String") return "String";
		}
		template<class T>
		T visit(AstNode* node) {
			if constexpr (std::is_same_v<T, void>) {
				if (node->print() == "Block") visit_Block(*static_cast<Block*>(node));
				else if (node->print() == "Assign") visit_Assign(*static_cast<class Assign*>(node));
				else if (node->print() == "NoOp") visit_NoOp();
				else if (node->print() == "Print") visit_Print(*static_cast<Print*>(node));
				else if (node->print() == "FunctionDecl") visit_FunctionDecl(*static_cast<FunctionDecl*>(node));
				else if (node->print() == "FunctionCall") visit_FunctionCall(*static_cast<FunctionCall*>(node));
				else {
					std::string error = "Error: void operation not recognized";
					throw error;
				}
			}
			else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, double>) {
				// cout << "Visiting number: " << node->print() << endl;
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
				else if (node->print() == "FunctionCall") {
					return visit_FunctionCall<T>(*static_cast<FunctionCall*>(node));
				}
				else {
					std::string error = "Error: not recognized";
					throw error;
				}
			}
			else if constexpr (std::is_same_v<T, std::string>) {
				if (node->print() == "Var")return visit_Var<T>(*static_cast<Var*>(node));
				else if (node->print() == "String") return visit_String(*static_cast<String*>(node));
				else if (node->print() == "BinOp") return visit_BinOp <std::string>(*static_cast<BinOp*>(node));
			}
			else {
				std::string error("Type not recognized");
				throw error;
			}
		}
		void visit_FunctionDecl(FunctionDecl functionDecl) {

		}
		template <typename T = void>
		T visit_FunctionCall(FunctionCall functionCall) {
			// cout << functionCall.type << endl;
			std::string func_name = functionCall.func_name;
			// cout << func_name << endl;
			ActivationRecord ar(ARType::FUNCTION, 2, func_name);
			FunctionSymbol func_symbol = functionCall.funcSymbol;
			// // cout << func_symbol.print() << endl;
			std::vector<VarSymbol> formal_params = func_symbol.params;
			std::vector<AstNode*> actual_params = functionCall.params;
			for (int i = 0; i < formal_params.size(); i++) {
				if (formal_params[i].type->name == "int") ar[formal_params[i].name] = visit<int>(actual_params[i]);
				else if (formal_params[i].type->name == "real") ar[formal_params[i].name] = visit<double>(actual_params[i]);
				else if (formal_params[i].type->name == "string") ar[formal_params[i].name] = visit<std::string>(actual_params[i]);
			}
			stack.push(ar);
			ar = stack.peek();
			// cout << std::get<int>(ar["bar"]) << endl;
			// cout << "AR Done!" << endl;
			// cout << std::get<int>(ar["bar"]) << endl;
			visit_Block(func_symbol.blockAst);
			// cout << "Block done!" << endl;
			if constexpr (!is_same_v<T, void>) {
				if (is_same_v<T, int>)
					if (functionCall.type != "int") {
						std::string error("You wanted int, but the function returns " + functionCall.type);
						throw error;
					}
				T ret = visit<T>(func_symbol.ret);
				// cout << ret << endl;
				stack.pop();
				return ret;
			}
			else stack.pop();
		}
		void visit_Print(Print p) {
			// cout << p.str.raw << endl;
			cout << visit_String(p.str);
			cout << endl;
		}
		std::string visit_String(String str) {
			int expr_index = 0;
			std::string result = "";
			for (int i = 0; i < str.raw.size(); i++) {
				// cout << str.raw[i];
				if (str.raw[i] == '{') {
					AstNode* node = str.expr[expr_index];
					// cout << boolalpha << (node == nullptr) << endl << node->print() << endl;
					expr_index++;
					// cout << getType(node) << endl;
					if (getType(node) == "int") {
						// cout << visit<int>(node) << endl;
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
			// cout << result << endl;
			return result;
		}
		template <class T>
		T visit_BinOp(BinOp binOp) {
			if constexpr (is_same_v<T, int>)
			{
				if (binOp.op.type == Div) {
					std::string error = "Error: Float division and int are incompatible types";
					throw error;
				}
				else if (binOp.op.type == Plus) {
					return (visit<int>(binOp.left) + visit<int>(binOp.right));
				}
				else if (binOp.op.type == Minus) {
					return (visit<int>(binOp.left) - visit<int>(binOp.right));
				}
				else if (binOp.op.type == Times) return (visit<int>(binOp.left) * visit<int>(binOp.right));
				else if (binOp.op.type == IntDiv) return (visit<int>(binOp.left) / visit<int>(binOp.right));
				else {
					std::string error("Error: BinOp operation not recognized.");
					throw error;
				}
			}
			else if constexpr (is_same_v<T, double>) {
				if (binOp.op.type == Plus) {
					return (visit<double>(binOp.left) + visit<double>(binOp.right));
				}
				else if (binOp.op.type == Minus) {
					return (visit<double>(binOp.left) - visit<double>(binOp.right));
				}
				else if (binOp.op.type == Times) return (visit<double>(binOp.left) * visit<double>(binOp.right));
				else if (binOp.op.type == IntDiv) return (visit<int>(binOp.left) / visit<int>(binOp.right));
				else if (binOp.op.type == Div) {
					return (visit<double>(binOp.left) / visit<double>(binOp.right));
				}
				else {
					std::string error("Error: BinOp operation not recognized.");
					throw error;
				}
			}
			else if constexpr (std::is_same_v<T, std::string>) {
				if (binOp.op.type == Plus) return visit<std::string>(binOp.left) + visit<std::string>(binOp.right);
				else if (binOp.op.type == Times) {
					if (getType(binOp.left) == "int") {
						int times = visit<int>(binOp.left);
						std::string res = "";
						std::string toAdd = visit<std::string>(binOp.right);
						for (int i = 0; i < times; i++) {
							res += toAdd;
						}
						return res;
					}
					else {
						int times = visit<int>(binOp.right);
						std::string res = "";
						std::string toAdd = visit<std::string>(binOp.left);
						for (int i = 0; i < times; i++) {
							res += toAdd;
						}
						return res;
					}
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
			if constexpr (is_same_v<T, int>) {
				if (op == Plus) return (visit<int>(unOp.expr));
				else return (0 - visit<int>(unOp.expr));
			}
			else if (is_same_v<T, double>) {
				if (op == Plus) return (visit<double>(unOp.expr));
				else return (0 - visit<double>(unOp.expr));
			}
			else {
				std::string error("Type not recognized");
				throw error;
			}
		}
		void visit_Block(Block block) {
			ActivationRecord ar = stack.peek();
			// cout << std::get<int>(ar["bar"]) << endl;
			// cout << "Visiting block..." << endl;
			for (AstNode* child : block.children) {
				// cout << "Visiting child..." << endl;
				// cout << child->print() << endl;
				visit<void>(child);
			}
		}
		void visit_Assign(class Assign assign) {
			std::string type = getType(&assign.var);
			std::string var_name = assign.var.value;
			// cout << stack.records.size() << endl;
			if (type == "int") {
				int res = visit<int>(assign.right);
				ActivationRecord& ar = stack.peek();
				ar[var_name] = res;
				// cout << std::get<int>(ar[var_name]);
				return;
			}
			else if (type == "real") {
				double res = visit<double>(assign.right);
				ActivationRecord& ar = stack.peek();
				ar[var_name] = res;
				return;
			}
			else if (type == "string") {
				std::string res = visit<std::string>(static_cast<String*>(assign.right));
				ActivationRecord& ar = stack.peek();
				ar[var_name] = res;
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
			// if (ar.name == "foo") cout << std::get<int>(ar["bar"]) << endl;
			if (ar.members.find(var.value) != ar.members.end()) {
				if constexpr (std::is_same_v<T, int>) {
					std::string type = getType(&var);
					if (type == "int") return (T)std::get<int>(ar[var.value]);
					else {
						std::string error("Wanted integer, got " + type);
						throw error;
					}
				}
				else if constexpr (std::is_same_v<T, double>) {
					std::string type = getType(&var);
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
					std::string type = getType(&var);
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
			SemanticAnalyzer symtabBuilder("global", 1);
			//std::// cout << "Building symtab...\n";
			try {
				symtabBuilder.visit(&p);
			}
			catch (std::string error) {
				cout << error << endl;
				return;
			}
			//std::// cout << "Finished building symtab...\n";
			//std::// cout << "Interpreting...\n";
			ARType t = ARType::PROGRAM;
			ActivationRecord ar(t, 1);
			stack.push(ar);
			try {
				visit_Block(p.block);
			}
			catch (std::string error) {
				cout << error << "\n";
				return;
			}
			
			stack.pop();
		}
	};
}