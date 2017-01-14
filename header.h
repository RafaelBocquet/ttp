// -*- compile-command: "make header.pch"; -*-
#pragma once
#include <stdio.h>
#include <bits/stdc++.h>

#define FOR(i, n)     for(int i = 0; i < (int)(n); ++i)
#define FORU(i, j, k) for(int i = (j); i <= (int)(k); ++i)
#define FORD(i, j, k) for(int i = (j); i >= (int)(k); --i)

#define SQ(x) ((x)*(x))

#define all(x) begin(x), end(x)
#define rall(x) rbegin(x), rend(x)
#define mp make_pair
#define mt make_tuple
#define pb push_back
#define eb emplace_back

using namespace std;

template<typename... As>
struct tpl : public std::tuple<As...> {
  using std::tuple<As...>::tuple;

  template<typename T = tuple<As...> >
  typename tuple_element<0, T>::type const&
  x() const { return get<0>(*this); }
  template<typename T = tuple<As...> >
  typename tuple_element<0, T>::type&
  x() { return get<0>(*this); }

  template<typename T = tuple<As...> >
  typename tuple_element<1, T>::type const&
  y() const { return get<1>(*this); }
  template<typename T = tuple<As...> >
  typename tuple_element<1, T>::type&
  y() { return get<1>(*this); }

  template<typename T = tuple<As...> >
  typename tuple_element<2, T>::type const&
  z() const { return get<2>(*this); }
  template<typename T = tuple<As...> >
  typename tuple_element<2, T>::type&
  z() { return get<2>(*this); }

  template<typename T = tuple<As...> >
  typename tuple_element<3, T>::type const&
  w() const { return get<3>(*this); }
  template<typename T = tuple<As...> >
  typename tuple_element<3, T>::type&
  w() { return get<3>(*this); }
};

using pii   = tpl<int, int>;
using piii  = tpl<int, int, int>;
using piiii = tpl<int, int, int, int>;
using vi    = vector<int>;
using vii   = vector<pii>;
using viii  = vector<piii>;
using vvi   = vector<vi>;
using vvii  = vector<vii>;
using vviii = vector<viii>;

template<class T>
using min_queue = priority_queue<T, vector<T>, greater<T> >;
template<class T>
using max_queue = priority_queue<T>;

template<class T, size_t... I>
void print_tuple(ostream& s, T const& a, index_sequence<I...>){
  using swallow = int[];
  (void)swallow{0, (void(s << (I == 0? "" : ", ") << get<I>(a)), 0)...};
}

template<class T>
ostream& print_collection(ostream& s, T const& a);
template<class... A>
ostream& operator<<(ostream& s, tpl<A...> const& a);
template<class... A>
ostream& operator<<(ostream& s, tuple<A...> const& a);
template<class A, class B>
ostream& operator<<(ostream& s, pair<A, B> const& a);

template<class T, size_t I>
ostream& operator<<(ostream& s, array<T, I> const& a) { return print_collection(s, a); }
template<class T>
ostream& operator<<(ostream& s, vector<T> const& a) { return print_collection(s, a); }
template<class T, class U>
ostream& operator<<(ostream& s, multimap<T, U> const& a) { return print_collection(s, a); }
template<class T>
ostream& operator<<(ostream& s, multiset<T> const& a) { return print_collection(s, a); }
template<class T, class U>
ostream& operator<<(ostream& s, map<T, U> const& a) { return print_collection(s, a); }
template<class T>
ostream& operator<<(ostream& s, set<T> const& a) { return print_collection(s, a); }

template<class T>
ostream& print_collection(ostream& s, T const& a){
  s << '[';
  for(auto it = begin(a); it != end(a); ++it){
    s << *it;
    if(it != prev(end(a))) s << " ";
  }
  return s << ']';
}

template<class... A>
ostream& operator<<(ostream& s, tpl<A...> const& a){
  s << '(';
  print_tuple(s, a, index_sequence_for<A...>{});
  return s << ')';
}

template<class... A>
ostream& operator<<(ostream& s, tuple<A...> const& a){
  s << '(';
  print_tuple(s, a, index_sequence_for<A...>{});
  return s << ')';
}

template<class A, class B>
ostream& operator<<(ostream& s, pair<A, B> const& a){
  return s << "(" << get<0>(a) << ", " << get<1>(a) << ")";
}

// Random

static struct {
  uint32_t x = 1, y = 2, z = 3, w = 4;
  inline void reset(uint32_t seed) {
    x = seed; y = seed+1; z = seed+2; w = seed+3;
  }
  inline uint32_t xorshift128() {
    uint32_t t = x;
    t ^= t << 11;
    t ^= t >> 8;
    x = y; y = z; z = w;
    w ^= w >> 19;
    w ^= t;
    return w;
  }
} xorshift128_s;

inline uint32_t xorshift128() {
  return xorshift128_s.xorshift128();
}

inline void random_reset(uint32_t seed) {
  xorshift128_s.reset(seed);
}

inline int random(int N) {
  return xorshift128()%N;
}

inline double randomDouble() {
  return (double)xorshift128()/4294967296.0;
}

// Terminal related

#define P_BEGIN   "\033[2K\r" // Used to get back to the beginning of the line, e.g. for a progress counter
#define C_RED     "\033[31m" // Colors
#define C_GREEN   "\033[32m"
#define C_YELLOW  "\033[33m"
#define C_BLUE    "\033[34m"
#define C_MAGENTA "\033[35m"
#define C_CYAN    "\033[36m"
#define C_BRED     "\033[31;1m" // Bright
#define C_BGREEN   "\033[32;1m"
#define C_BYELLOW  "\033[33;1m"
#define C_BBLUE    "\033[34;1m"
#define C_BMAGENTA "\033[35;1m"
#define C_BCYAN    "\033[36;1m"
#define C_RESET   "\033[0m"  // Reset the terminal state

// Time

static struct timeInfoS {
  chrono::high_resolution_clock::time_point c_start;
  timeInfoS(){ c_start = chrono::high_resolution_clock::now(); }
  double getTime() {
    chrono::high_resolution_clock::time_point c_now =
      chrono::high_resolution_clock::now();
    return chrono::duration<double>(c_now-c_start).count();
  }
} timeInfo;

// Union-find

struct union_find {
  vi A;

  union_find(int n = 0) : A(n) {
    iota(all(A), 0);
  }

  int addNode() {
    A.pb(A.size());
    return A.size()-1;
  }

  int find(int a) {
    return A[a] == a ? a : A[a] = find(A[a]);
  }

  void unite(int a, int b) {
    a = find(a); b = find(b);
    A[a] = b;
  }
};
