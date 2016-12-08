// -*- compile-command: "make all"; -*-
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
    return sqrt(dist2(i,j));
  }
} I;

struct solution {
  solution() {
    n = I.n; m = I.m;
  }
  int n, m;
  //
  int TW = 0;
  vector<int>  A;
  vector<bool> B;
  //
  void print(ostream& ss) {
    ss << "[1,"; FOR(i,I.n-1) { ss << ',' << A[i+1]+1; } ss << ']' << '\n';
    { bool b=0;
      ss << '['; FOR(i,I.m) if(B[i]) { if(b) ss << ','; b=1; ss << i+1; } ss << ']' << '\n';
    }
  }
  //
  void trivial() {
    A.resize(n-1); iota(all(A),1);
    B.resize(m,0);
  }
  //
  double score() {
    double score = 0;
    //
    FOR(i,I.m) if(B[i]) {
      score += I.items[i].p;
    }
    //
    int cw = 0;
    FOR(i0,A.size()) {
      int i = A[i0];
      int j = (i0+1 < (int)A.size()) ? A[i0+1] : 0;
      for(int k : I.cities[i].items) if(B[k]) cw += I.items[k].w;
      double d = I.dist(i,j);
      score -= (I.R * d) / (I.vmax - I.nu * cw);
    }
    return score;
  }
};

void sa() {
  int n = I.n, m = I.m;
  solution S; S.trivial(); double curScore = S.score();
  solution bestS = S; double bestScore = curScore;
  //
  auto add = [&](){
    if(curScore > bestScore) {
      bestS = S;
      bestScore = curScore;
      cerr << "Improve: " << bestScore << endl;
    }
  };
  //
  auto trySwap = [&](){
    int i=0, j=0;
    while(i==j) { i = random(n-1); j = random(n-1); }
    swap(S.A[i], S.A[j]);
    double score = S.score();
    if(score > curScore) {
      curScore = score;
      add();
    }else{
      swap(S.A[i], S.A[j]);
    }
  };
  auto tryFlip = [&](){
    int i = random(m);
    S.B[i] = !S.B[i]; S.TW += (S.B[i]?1:-1)*I.items[i].w;
    double score = S.score();
    if(score > curScore && S.TW < I.W) {
      curScore = score;
      add();
    }else{
      S.B[i] = !S.B[i]; S.TW += (S.B[i]?1:-1)*I.items[i].w;
    }
  };
  //
  int niter=0;
  while(1) {
    niter += 1;
    if((niter&65535) == 0) {
      cout << niter << endl;
    }
    int r = random(2);
    if(r<1) {
      trySwap();
    }else{
      tryFlip();
    }
  }
}

int main(int argc, char** argv){
  assert(argc >= 2);
  char *filename = argv[1];

  double time_0 = timeInfo.getTime();

  { ifstream F(filename);
    I.read(F);
    if(!I.validateAndInit()) {
      throw runtime_error("Missing data in file");
    }
  }
  I.print();

  sa();

  cerr << "Elapsed: " << (timeInfo.getTime()-time_0) << "s" << endl;

  return 0;
}
