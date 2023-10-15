#include <functional>

using namespace std;

template <typename T>
using π = function<bool(T)>;  // π for predicate

template <typename T>
π<T> is(T a) {
  return [a](T b) { return b == a; };
}

template <typename T>
π<T> τ(T a) {  // τ for true
  return [](T b) { return true; };
}
