#include <cassert>
#include <iostream>

#include "test_simple_fsm.cpp"
// #include "test_lisp_parser.cpp"

// void test_lisp_parser() {
//   fsm<char, Node*>* p = sexpr_parser(new Node(LIST));

//   std::string input = "(+ 2 (* 3 4))";
//   for (char c : input) {
//     *p <<= c;
//   }

//   std::cout << "Parsed: " << input << std::endl;
//   auto result = p->state()->eval();
//   std::cout << "Resulting State: " << result << std::endl;
//   assert(result == 14);
// }

int main() {
  // test_lisp_parser();
  test_simple();
  return 0;
}
