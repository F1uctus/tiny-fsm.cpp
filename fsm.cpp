#include <functional>
#include <stack>
#include <unordered_map>

#include "higher.cpp"

using namespace std;

template <typename Σ, typename S>
class fsm;

template <typename Σ, typename S>
fsm<Σ, S>& operator<<(fsm<Σ, S>&, Σ);

/// @brief https://en.wikipedia.org/wiki/Finite-state_machine#Mathematical_model
/// @tparam Σ is the input alphabet (a finite non-empty type of symbols);
/// @tparam S is a finite non-empty type of states.
template <typename Σ, typename S>
class fsm {
 public:
  // State-transition function types
  using δ = function<S(S, Σ)>;
  using γ = function<fsm*(S, Σ)>;

  fsm(S initial, π<S> is_terminal)
      : current(initial), is_terminal(is_terminal) {
    machine_stack = new stack<fsm*>();
    machine_stack->push(this);
  }

#pragma region Simple transition registration

  δ give(S value) {  // S -> (S⨯Σ -> S)
    return [value](S, Σ) { return value; };
  }

  void on(Σ in, S from, S to) { return on(is(in), is(from), give(to)); }

  void on(set<Σ> in, S from, S to) { return on(is(in), is(from), give(to)); }

  void on(π<Σ> in, S from, S to) { return on(in, is(from), give(to)); }

  void on(Σ in, π<S> from, S to) { return on(is(in), from, give(to)); }

  void on(set<Σ> in, π<S> from, S to) { return on(is(in), from, give(to)); }

  void on(π<Σ> in, π<S> from, S to) { return on(in, from, give(to)); }

  void on(Σ in, S from, δ t) { return on(is(in), is(from), t); }

  void on(set<Σ> in, S from, δ t) { return on(is(in), is(from), t); }

  void on(π<Σ> in, S from, δ t) { return on(in, is(from), t); }

  void on(Σ in, π<S> from, δ t) { return on(is(in), from, t); }

  void on(set<Σ> in, π<S> from, δ t) { return on(is(in), from, t); }

  void on(π<Σ> in, π<S> from, δ t) {
    transitions.push_back(make_tuple(in, from, t));
  }

#pragma endregion

#pragma region Submachine transition registration

  void on(Σ in, π<S> from, γ t) { return on(is(in), from, t); }

  void on(π<Σ> in, π<S> from, γ t) {
    submachines.push_back(make_tuple(in, from, t));
  }

#pragma endregion

  // Process an input and transition to the new state
  friend fsm& operator<< <>(fsm& m, Σ input);

  S state() const { return current; }

 private:
  S current;
  π<S> is_terminal;
  vector<tuple<π<Σ>, π<S>, δ>> transitions;
  vector<tuple<π<Σ>, π<S>, γ>> submachines;
  stack<fsm*>* machine_stack;
};

template <typename Σ, typename S>
fsm<Σ, S>& operator<<(fsm<Σ, S>& m, Σ input) {
  bool found = false;
  // Handle upper-level transitions
  S new_state;
  for (auto tup : m.transitions) {
    auto input_pred = get<0>(tup);
    auto state_pred = get<1>(tup);
    if (state_pred(m.current) && input_pred(input)) {
      new_state = get<2>(tup)(m.current, input);
      found = true;
    }
  }
  if (found) {
    m.current = new_state;
    if (m.is_terminal(m.current)) {
      m.machine_stack->pop();
      return *m.machine_stack->top();
    }
    return m;
  }
  // Handle submachine transitions
  fsm<Σ, S>* sub;
  for (auto tup : m.submachines) {
    auto input_pred = get<0>(tup);
    auto state_pred = get<1>(tup);
    if (state_pred(m.current) && input_pred(input)) {
      sub = get<2>(tup)(m.current, input);
      found = true;
    }
  }
  if (found) {
    m.machine_stack->push(sub);
    sub->machine_stack = m.machine_stack;
    return *m.machine_stack->top();
  }
  throw "Invalid input";
}
