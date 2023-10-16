#include <cassert>
#include <iostream>
#include <map>
#include <sstream>

#include "fsm.cpp"

enum LispState {
  LIST,
  COMPLETE_LIST,
  SYMBOL,
  NUMBER,
  OPERATOR,
};

struct Node {
  LispState type;
  vector<Node*> children = {};
  std::stringstream value = {};
  Node* parent = nullptr;

  Node(LispState type) : type(type) {}

  int eval() {
    switch (type) {
      case NUMBER:
        return stoi(value.str());
      case LIST:
      case COMPLETE_LIST: {
        assert(children.size() == 1);
        return children.front()->eval();
      }
      case OPERATOR: {
        assert(!children.empty());
        auto a = children.front()->eval();
        auto op = map<char, function<int(int)>>{
            {'+', [a](int x) { return a + x; }},
            {'-', [a](int x) { return a - x; }},
            {'*', [a](int x) { return a * x; }},
            {'/', [a](int x) { return a / x; }},
        }[value.str().front()];
        for (auto ch = children.begin()++; ch != children.end(); ch++) {
          a = op((*ch)->eval());
        }
        return a;
      }
    }
    return 0;
  }
};

std::function<bool(Node*)> when(std::set<LispState> s) {
  return [s](Node* n) { return is(s)(n->type); };
};

Node* pass(Node* n, char) { return n; };

Node* pop(Node* n, char) { return n->parent; };

Node* concat(Node* n, char c) {
  n->value << c;
  return n;
};

std::function<Node*(Node*, char)> mark_child(LispState s) {
  return [s](Node* parent, char c) {
    auto child = new Node(s);
    child->parent = parent;
    child->value << c;
    parent->children.push_back(child);
    return child;
  };
};

// For compatibility purposes only.
std::function<bool(char)> alpha = [](char c) { return std::isalpha(c); };
std::function<bool(char)> digit = [](char c) { return std::isdigit(c); };

fsm<char, Node*>* sexpr_parser(Node* parent) {
  // Declare S-Expression parser
  auto p = new fsm<char, Node*>(parent, when({COMPLETE_LIST}));
  p->on('(', any<Node*>(), [](Node* parent, char c) {
    return sexpr_parser(mark_child(LIST)(parent, c));
  });
  p->on(')', any<Node*>(), [](Node* n, char c) {
    while (n->type != LIST && n->parent) n = n->parent;
    n->type = COMPLETE_LIST;
    return n;
  });
  p->on(' ', when({OPERATOR}), pass);
  p->on(' ', no(when({OPERATOR})), pop);
  // Symbols
  p->on(alpha, when({LIST, OPERATOR}), mark_child(SYMBOL));
  p->on(alpha, when({SYMBOL}), concat);
  // Numbers
  p->on(digit, when({LIST, OPERATOR}), mark_child(NUMBER));
  p->on(digit, when({NUMBER}), concat);
  // Operators
  p->on({'+', '-', '*', '/'}, when({LIST}), mark_child(OPERATOR));
  p->on({'+', '-', '*', '/'}, when({OPERATOR}), concat);
  return p;
}
