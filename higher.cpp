#include <functional>
#include <set>

using namespace std;

template <typename T>
using π = function<bool(T)>;  // π for predicate

template <typename T>
π<T> is(T a) {
  return [a](T b) { return b == a; };
}

template <typename T>
π<T> no(T a) {
  return [a](T b) { return b != a; };
}

template <typename T>
π<T> no(set<T> a) {
  return [a](T b) { return a.find(b) == a.end(); };
}

template <typename T>
π<T> is(set<T> a) {
  return [a](T b) { return a.find(b) != a.end(); };
}

template <typename T>
π<T> no(π<T> inner) {
  return [inner](T x) { return !inner(x); };
}

template <typename T>
π<T> any() {
  return [](T b) { return true; };
}
