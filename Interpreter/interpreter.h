#pragma once
#include "../Parser/parser.h"
#include <map>
#include <string>
class Interpreter
{
public:
	std::map<std::string, double> GLOBAL_SCOPE;
	Interpreter(std::string input) : parser(input) {
	}
	~Interpreter() {}
	void visit(AstNode* node) {
		if (node->print() == "Block") visit_Block(*static_cast<Block*>(node));
		else if (node->print() == "Assign") visit_Assign(*static_cast<class Assign*>(node));
		else if (node->print() == "NoOp") visit_NoOp();
		else {
			std::string error = "Error: not recognized";
			throw error;
		}
	}
	double visit_Real(AstNode* node) {
		if (node->print() == "Num") return visit_Num(*static_cast<Num*>(node));
		else if (node->print() == "UnOp") return visit_UnOp(*static_cast<UnOp*>(node));
		else if (node->print() == "Var") return visit_Var(*static_cast<Var*>(node));
		else if (node->print() == "BinOp") return visit_BinOp(*static_cast<BinOp*>(node));
		else {
			std::string error = "Error: not recognized";
			throw error;
		}
	}
	double visit_Num(Num num) {
		return num.value;
	}
	double visit_BinOp(BinOp binOp) {
		if (binOp.op.type == Plus) return visit_Real(binOp.left) + visit_Real(binOp.right);
		else if (binOp.op.type == Minus) return visit_Real(binOp.left) - visit_Real(binOp.right);
		else if (binOp.op.type == Times) return visit_Real(binOp.left) * visit_Real(binOp.right);
		else if (binOp.op.type == Div) return (double)(visit_Real(binOp.left) / (double)visit_Real(binOp.right));
		else if (binOp.op.type == IntDiv) return (int)(visit_Real(binOp.left) / visit_Real(binOp.right));
		else {
			std::string error("Error: BinOp operation not recognized.");
			throw error;
		}
	}
	double visit_UnOp(UnOp unOp) {
		TokenType op = unOp.op.type;
		if (op == Plus) return visit_Real(unOp.expr);
		else return 0 - visit_Real(unOp.expr);
	}
	void visit_Block(Block block) {
		for (AstNode* child : block.children) {
			visit(child);
		}
	}
	void visit_Assign(class Assign assign) {
		string var_name = assign.var.value;
		GLOBAL_SCOPE[var_name] = visit_Real(assign.right);
	}
	double visit_Var(Var var) {
		if (GLOBAL_SCOPE.find(var.value) != GLOBAL_SCOPE.end()) return GLOBAL_SCOPE[var.value];
	}
	void visit_NoOp() {}
	void interpret() {
		Block block = parser.parseProgram();
		visit_Block(block);
	}
private:
	Parser parser;
};