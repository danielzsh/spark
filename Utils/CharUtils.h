#pragma once

#include <unordered_set> // for unordered_set
#include <algorithm> // for isalpha and isdigit

// Way better than if statement (for optimization & caching)
bool inArray(char query, std::unordered_set<char> &set) {
    // set.find Time complexity: O(log n) - Logarithmic time, better than vector or for loop (O(n))
    return set.find(query) != set.end();
}

namespace CharUtils {
  bool isLetter (const char &query) {
      return std::isalpha(query);
  }
  bool isDigit (const char &query) {
        return std::isdigit(query);
  }
  bool isParenthesis (const char &query) {
      std::unordered_set<char> s = {'(', ')'};
      return inArray(query, s);
  }
  bool isOperator (char query) {
      std::unordered_set<char> s = {'+', '-', '*', '/', '>', '<', '='};
      return inArray(query, s);
  }
  bool isArithmeticOperator (char query) {
      std::unordered_set<char> s = {'+', '-', '*', '/'};
      return inArray(query, s);
  }
  bool isComparisonOperator (char query) {
      std::unordered_set<char> s = {'>', '<', '='};
      return inArray(query, s);
  }
  bool isWhitespaceOrNewline (char query) {
        std::unordered_set<char> s = {' ', '\t', '\n'};
        return inArray(query, s);
  }
  bool isNewLine (char query) {
    return query == '\n';
  }
}