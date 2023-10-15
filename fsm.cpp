#include <functional>
#include <unordered_map>

#include "higher.cpp"

using namespace std;

/// @brief https://en.wikipedia.org/wiki/Finite-state_machine#Mathematical_model
/// @tparam Σ is the input alphabet (a finite non-empty type of symbols);
/// @tparam S is a finite non-empty type of states.
template <typename Σ, typename S>
class fsm {
 public:
  // State-transition function type
  using δ = function<S(S, Σ)>;

  fsm(S initial, S terminal) : current(initial), terminal(terminal) {}

#pragma region Simple transition registration

  δ give(S value) {  // S -> (S⨯Σ -> S)
    return [value](S, Σ) { return value; };
  }

  void on(Σ in, S from, S to) { return on(is(in), is(from), give(to)); }

  void on(π<Σ> in, S from, S to) { return on(in, is(from), give(to)); }

  void on(Σ in, π<S> from, S to) { return on(is(in), from, give(to)); }

  void on(π<Σ> in, π<S> from, S to) { return on(in, from, give(to)); }

  void on(Σ in, S from, δ t) { return on(is(in), is(from), t); }

  void on(π<Σ> in, S from, δ t) { return on(in, is(from), t); }

  void on(Σ in, π<S> from, δ t) { return on(is(in), from, t); }

  void on(π<Σ> in, π<S> from, δ t) {
    transitions.push_back(make_tuple(in, from, t));
  }

#pragma endregion

#pragma region Submachine transition registration

  void sub(Σ in, S from, fsm<Σ, S>* m) { return sub(is(in), is(from), m); }

  void sub(π<Σ> in, S from, fsm<Σ, S>* m) { return sub(in, is(from), m); }

  void sub(Σ in, π<S> from, fsm<Σ, S>* m) { return sub(is(in), from, m); }

  void sub(π<Σ> in, π<S> from, fsm<Σ, S>* m) {
    submachines.push_back(make_tuple(in, from, m));
  }

#pragma endregion

  // Process an input and transition to the new state
  friend fsm<Σ, S>& operator<<(fsm<Σ, S>& m, Σ input) {
    // Check for upper-level transitions
    S* new_state;
    for (auto table : m.transitions) {
      auto input_pred = get<0>(table);
      auto state_pred = get<1>(table);
      if (state_pred(m.current) && input_pred(input)) {
        S value = get<2>(table)(m.current, input);
        new_state = &value;
      }
    }
    if (new_state) {
      m.current = *new_state;
      if (m.current == m.terminal) {
        // TODO: return (FSM stack).pop();
      }
      return m;
    }
    // Check for submachine transitions
    fsm<Σ, S>* sub;
    for (auto table : m.submachines) {
      auto input_pred = get<0>(table);
      auto state_pred = get<1>(table);
      if (state_pred(m.current) && input_pred(input)) {
        sub = get<2>(table);
      }
    }
    if (sub) {
      (*sub) << input;
      return *sub;
    }
    throw "Invalid input";
  }

  S currentState() const { return current; }

 private:
  S current;
  S terminal;
  vector<tuple<π<Σ>, π<S>, δ>> transitions;
  vector<tuple<π<Σ>, π<S>, fsm<Σ, S>*>> submachines;
};