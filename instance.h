// -*- compile-command: "make all"; -*-
#pragma once
#include "header.h"

bool startsWith(string const& s, string const& t) {
  if(s.size() < t.size()) return 0;
  FOR(i,t.size()) if(s[i]!=t[i]) return 0;
  return 1;
}

struct city {
  int x, y;
  vi items;
};

struct item {
  int p, w;
  int node;
};

struct instance {
  string name = "";
  int n = -1, m = -1, W = -1;
  double vmin = -1, vmax = -1, R;

  double nu = -1;

  vector<city> cities;
  vector<item> items;

  void read(istream& ss) {
    while(1) {
      while(ss.good() && (isblank(ss.peek()) || iscntrl(ss.peek()))) ss.get();
      if(!ss.good()) return;
      string s; getline(ss, s);
      char *buf0 = 0, *buf1 = 0;
      if(sscanf(s.data(), " %m[^:\r\n]: %m[^:\r\n] ", &buf0, &buf1) == -1) {
        throw runtime_error("Bad line: " + s);
      }
      string s0(buf0);
      if(s0 == "PROBLEM NAME") {
        name = string(buf1);
      } else if(s0 == "KNAPSACK DATA TYPE") {
      } else if(s0 == "DIMENSION") {
        n = atoi(buf1);
      } else if(s0 == "NUMBER OF ITEMS") {
        m = atoi(buf1);
      } else if(s0 == "CAPACITY OF KNAPSACK") {
        W = atoi(buf1);
      } else if(s0 == "MIN SPEED") {
        vmin = atof(buf1);
      } else if(s0 == "MAX SPEED") {
        vmax = atof(buf1);
      } else if(s0 == "RENTING RATIO") {
        R = atof(buf1);
      } else if(s0 == "EDGE_WEIGHT_TYPE") {
        assert(string(buf1) == "CEIL_2D");
      } else if(startsWith(s0, "NODE_COORD_SECTION")) {
        cities.resize(n); int dummy;
        FOR(i,n) { ss >> dummy >> cities[i].x >> cities[i].y;  }
      } else if(startsWith(s0, "ITEMS SECTION")) {
        items.resize(m); int dummy;
        FOR(i,m) {
          ss >> dummy >> items[i].p >> items[i].w >> items[i].node;
          --items[i].node;
          cities[items[i].node].items.pb(i);
        }
      } else {
        throw runtime_error("Unknown field: " + s0);
      }
      free(buf0); free(buf1);
    }
  }

  void print() {
    cerr << "Name: " << name << endl;
    cerr << "Cities: " << n << " " << "Items: " << m << endl;
    cerr << "Vmin: " << vmin << " Vmax: " << vmax << " " << endl;
  }

  bool validateAndInit() {
    if(name.empty()) return 0;
    if(n < 0 || m < 0 || W < 0 || R < 0) return 0;
    if(vmin < 0 || vmax < 0) return 0;

    nu = (vmax-vmin) / W;

    return 1;
  }

  double dist2(int i, int j) const {
    return SQ((double)(cities[i].x-cities[j].x)) + SQ((double)(cities[i].y-cities[j].y));
  }

  double dist(int i, int j) const {
    return ceil(sqrt(dist2(i,j)));
  }
} I;

template<class T>
struct llist {
  T value;
  llist<T> *n = 0, *p = 0;
};

struct partial_solution {
  vector<int>  A; // permutation
  vector<bool> B; // selected items
};

struct solution : partial_solution {
  solution() {
    n = I.n; m = I.m;

    A.resize(n-1); iota(all(A),1);
    B.resize(m,0);

    cityWeights.resize(n,0);

    itemList.resize(m); FOR(i,m) itemList[i].value = i;
    itemListSentinel.n = &itemListSentinel;
    itemListSentinel.p = &itemListSentinel;
  }

  void from_partial(partial_solution const& S) {
    A = S.A;
    B.assign(m,0);
    cityWeights.assign(n,0);
    itemListSentinel.n = &itemListSentinel;
    itemListSentinel.p = &itemListSentinel;
    FOR(i,m) if(S.B[i]) flipB(i);
  }

  //
  int n, m;
  //
  int TW = 0;     // totalo weight
  //
  vector<double> cityWeights;

  // careful : not valid after a copy
  // todo : ints instead of pointers
  vector<llist<int> > itemList;
  llist<int>          itemListSentinel;

  //
  void print(ostream& ss) {
    ss << "[1,"; FOR(i,I.n-1) { ss << ',' << A[i+1]+1; } ss << ']' << '\n';
    { bool b=0;
      ss << '['; FOR(i,I.m) if(B[i]) { if(b) ss << ','; b=1; ss << i+1; } ss << ']' << '\n';
    }
  }
  //
  double tourLength() {
    double len = 0;
    len -= I.dist(0,A[0]);
    FOR(i0,A.size()) {
      int i = A[i0];
      int j = (i0+1 < (int)A.size()) ? A[i0+1] : 0;
      double d = I.dist(i,j);
      len -= d;
    }
    return len;
  }
  //
  double score() {
    double score = 0;
    //
    { llist<int> *l = itemListSentinel.n;
      while(l != &itemListSentinel) {
        score += I.items[l->value].p;
        l = l->n;
      }
    }
    //
    int cw = 0;
    score -= (I.R * I.dist(0,A[0])) / I.vmax;
    FOR(i0,A.size()) {
      int i = A[i0];
      int j = (i0+1 < (int)A.size()) ? A[i0+1] : 0;
      cw += cityWeights[i];
      double d = I.dist(i,j);
      score -= (I.R * d) / (I.vmax - I.nu * cw);
    }
    return score;
  }
  //
  void flipB(int i) {
    if(B[i]) {
      B[i] = 0;
      TW -= I.items[i].w;
      cityWeights[I.items[i].node] -= I.items[i].w;
      itemList[i].n->p = itemList[i].p;
      itemList[i].p->n = itemList[i].n;
    }else{
      B[i] = 1;
      TW += I.items[i].w;
      cityWeights[I.items[i].node] += I.items[i].w;
      itemList[i].n = itemListSentinel.n;
      itemList[i].p = &itemListSentinel;
      itemList[i].n->p = &itemList[i];
      itemListSentinel.n = &itemList[i];
    }
  }
};
