#pragma once
#include <string>
#include <variant>
#include <map>
#include <vector>
enum class ARType {
	PROGRAM,
	FUNCTION
};
using value = std::variant<int, double, std::string>;
class ActivationRecord {
public:
	std::string name;
	ARType type;
	int nesting_level;
	std::map<std::string, value> members;
	ActivationRecord(ARType type, int nesting_level, std::string name = "") {
		this->name = name;
		this->type = type;
		this->nesting_level = nesting_level;
		this->members = members;
	}
	value& operator [] (std::string index) {
		return members[index];
	}
};
class CallStack {
public:
	std::vector<ActivationRecord> records;
	void push(ActivationRecord& ar) {
		records.push_back(ar);
	}
	void pop() {
		// cout << "The stack size is: " << records.size() << endl;
		records.pop_back();
	}
	ActivationRecord& peek() {
		// if (records.size() == 0) cout << "THERE ARE NO MORE RECORDS\n";
		return records.back();
	}
};