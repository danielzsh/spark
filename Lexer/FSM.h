#pragma once

#include <iostream>
#include <vector>
#include <string>

using namespace std;

template <typename T>
bool has (vector<T> vect, T query) {
  for (int i = 0; i < vect.size(); i++) {
    if (vect[i] == query) return true;
  }
  return false;
}

class FSM {
  private:
    vector<int> states;
    vector<int> acceptingStates;
    int initialState;
    int (*nextState)(int, char);
  public:
    int endState;
    FSM(vector<int> statesPass, vector<int> acceptingStatesPass, int initialStatePass, int nextStatePass(int, char)) {
      states = statesPass;
      acceptingStates = acceptingStatesPass;
      initialState = initialStatePass;
      nextState = nextStatePass;
    }
    string run (string input) {
      int currentState = initialState;
      string val = "";
      int i = 0; 
      for (; i < input.length(); i++) {
        char character = input[i];
        int NextState = nextState(currentState, character);
        if (NextState == 0) {
          break;
        }
        val += input[i];
        currentState = NextState;
      }
      endState = currentState;
      return val;
    } 

};