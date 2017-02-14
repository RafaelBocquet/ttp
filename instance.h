// -*- compile-command: "make main"; -*-
#pragma once
// #include "header.h" -- precompiled

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

  inline double dist2(int i, int j) const {
    return SQ((double)(cities[i].x-cities[j].x)) + SQ((double)(cities[i].y-cities[j].y));
  }

  inline double dist(int i, int j) const {
    return ceil(sqrt(dist2(i,j)));
  }
};
