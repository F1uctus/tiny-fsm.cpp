#include <cassert>
#include <iostream>

#include "fsm.cpp"

enum class States { A, B, C, D };
enum class Inputs { X, Y, Z, W };

void test_simple() {
  fsm<Inputs, States>* m;
  auto fsm1 = new fsm<Inputs, States>(States::A, is(States::D));
  auto fsm2 = new fsm<Inputs, States>(States::C, is(States::D));
  fsm2->on(Inputs::W, States::C, States::D);

  fsm1->on(Inputs::X, States::A, States::B);
  fsm1->on(Inputs::Y, States::B, States::C);
  fsm1->on(Inputs::Z, States::C, [fsm2](...) { return fsm2; });

  m = fsm1;
  cout << "Current State: " << static_cast<int>(m->state()) << endl;
  *m <<= Inputs::X;
  cout << "Current State: " << static_cast<int>(m->state()) << endl;
  *m <<= Inputs::Y;
  cout << "Current State: " << static_cast<int>(m->state()) << endl;
  *m <<= Inputs::Z;
  cout << "Current State: " << static_cast<int>(m->state()) << endl;
  *m <<= Inputs::W;
  cout << "Current State: " << static_cast<int>(m->state()) << endl;
  assert(m->state() == States::D);
}
