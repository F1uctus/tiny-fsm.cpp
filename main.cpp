#include <iostream>

#include "fsm.cpp"

using namespace std;

void test_lisp_parser();

int main() {
  test_lisp_parser();
  return 0;
}

enum class States { A, B, C, D };
enum class Inputs { X, Y, Z };

void test_abstract() {
  fsm<Inputs, States> fsm1(States::A, States::D);
  fsm<Inputs, States> fsm2(States::D, States::D);

  fsm1.on(Inputs::X, States::A, States::B);
  fsm1.on(Inputs::Y, States::B, States::C);
  fsm1.sub(Inputs::Z, States::C, &fsm2);

  cout << "Current State: " << static_cast<int>(fsm1.currentState()) << endl;
  fsm1 << Inputs::X;
  cout << "Current State: " << static_cast<int>(fsm1.currentState()) << endl;
  fsm1 << Inputs::Y;
  cout << "Current State: " << static_cast<int>(fsm1.currentState()) << endl;
  fsm1 << Inputs::Z;
  cout << "Current State: " << static_cast<int>(fsm1.currentState()) << endl;
}

enum class LispState { START, SYMBOL, NUMBER, LIST, OPERATOR, END };

void test_lisp_parser() {
  fsm<char, LispState> lisp_parser(LispState::START, LispState::END);

  // Define transitions for the Lisp parser
  lisp_parser.on('(', LispState::START, LispState::LIST);
  lisp_parser.on(')', LispState::LIST, LispState::START);
  lisp_parser.on(' ', LispState::START, LispState::START);
  lisp_parser.on(' ', LispState::SYMBOL, LispState::START);
  lisp_parser.on(' ', LispState::NUMBER, LispState::START);
  lisp_parser.on(' ', LispState::LIST, LispState::START);
  lisp_parser.on(' ', LispState::OPERATOR, LispState::START);

  // Define transitions for parsing symbols
  lisp_parser.on(isalpha, LispState::START, LispState::SYMBOL);
  lisp_parser.on(isalpha, LispState::SYMBOL, LispState::SYMBOL);

  // Define transitions for parsing numbers
  lisp_parser.on(isdigit, LispState::START, LispState::NUMBER);
  lisp_parser.on(isdigit, LispState::NUMBER, LispState::NUMBER);

  // Define transitions for parsing operators
  lisp_parser.on(
      [](char c) { return c == '+' || c == '-' || c == '*' || c == '/'; },
      LispState::START, LispState::OPERATOR);
  lisp_parser.on(
      [](char c) { return c == '+' || c == '-' || c == '*' || c == '/'; },
      LispState::OPERATOR, LispState::OPERATOR);

  std::string input = "(+ 2 (* 3 4))";
  for (char c : input) {
    lisp_parser << c;
  }

  std::cout << "Parsed: " << input << std::endl;
  std::cout << "Resulting State: "
            << static_cast<int>(lisp_parser.currentState()) << std::endl;
}
