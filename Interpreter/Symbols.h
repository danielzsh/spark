#pragma once
#include <string>
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
