#pragma once
#include "../Parser/parser.h"
#include <map>
#include <string>
class Interpreter
{
public:
	std::map<std::string, int> GLOBAL_SCOPE;
	Interpreter(std::string input) : parser(input) {
	}
	~Interpreter();
	void visit(AstNode* node) {
		if (node->print() == "Block") visit_Block(*static_cast<Block*>(node));
		else if (node->print() == "Assign") visit_Assign(*static_cast<Assign*>(node));
		else if (node->print() == "NoOp") visit_NoOp(*static_cast<NoOp*>(node));
		else if (node->print() == "Var") visit_Var(*static_cast<Var*>(node));
		else {
			std::string error = "Error: not recognized";
			throw error;
		}
	}
	int visit_Int(AstNode* node) {
		if (node->print() == "Num") return visit_Num(*static_cast<Num*>(node));
		else if (node->print == "UnOp") return visit_UnOp(*static_cast<UnOp*>(node));
		else {
			std::string error = "Error: not recognized";
			throw error;
		}
	}
	int visit_Num(Num num) {
		return num.value;
	}
	int visit_UnOp(UnOp unOp) {
		Token op = unOp.op.type;
		if (op.type == Plus) return visit_Int(unOp.expr);
		else return 0 - visit_Int(unOp.expr);
	}
	void visit_Block(Block block) {
		for (AstNode* child : block.children) {
			visit(child);
		}
	}
	void visit_Assign(class Assign assign) {
		string var_name = assign.var.value;
		GLOBAL_SCOPE.insert(pair<std::string, int>(name, visit_Int(assign.right)));
	}
	void visit_Var(Var var) {
		if (GLOBAL_SCOPE.find(var.value) != GLOBAL_SCOPE.end()) return GLOBAL_SCOPE[var.value];
		else {
			std::string error = "Error: variable not found";
			throw error;
		}
	}
	void visit_NoOp() {}
	void interpret() {
		Block block = parser.parseProgram();
		visit_Block(block);
	}
private:
	Parser parser;
};