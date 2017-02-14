// -*- compile-command: "make main"; -*-
// #include "header.h" -- precompiled
#include "instance.h"
#include "sa.h"
#include "tour.h"
#include "ttour.h"

const double MAX_SA_TIME1 = 30.0;
const double MAX_SA_TIME2 = 30.0;

// initial tour : 2-approximation of the tsp problem, O(n^2)
vi initial_tour(instance const& I) {
  cerr << "Computing initial tour..." << endl;
  double t0 = timeInfo.getTime();

  int n = I.n;
  vector<bool>   E(n); // explored
  vector<double> D2(n,1e100); // min distance squared
  vi             P(n,-1); // parent
  vvi            G(n); // mst
  int next=0;
  auto add = [&](int i) {
    E[i]=1; next=-1; double minD=1e100;
    FOR(j,n) if(!E[j]) {
      double d2 = I.dist2(i,j);
      if(d2 < D2[j]) {
        D2[j] = d2;
        P[j] = i;
      }
      if(D2[j]<minD) { minD = D2[j]; next=j; }
    }
  };
  FOR(i,n) {
    if(!(i&63)) cout << i << "/" << n << flush << P_BEGIN;
    if(P[next] != -1) G[P[next]].pb(next);
    add(next);
  }

  vi tour; tour.reserve(n-1);
  function<void(int)> dfs = [&](int i) {
    if(i) tour.pb(i);
    for(int j : G[i]) dfs(j);
  };
  dfs(0);
  assert((int)tour.size() == n-1);

  cout << "Done (" << timeInfo.getTime()-t0 << "s)" << endl;
  return tour;
}

double tour_length(instance const& I, vi const& tour) {
  int n = I.n;
  assert((int)tour.size() == n-1);
  double len = 0.0;
  int last=0;
  FOR(i,n-1) { len += I.dist(last,tour[i]); last=tour[i]; }
  len += I.dist(last,0);
  return len;
}

array_tour tsp(instance const& I, vi const& tour) {
  int n = I.n;
  array_tour S(tour);
  double score = tour_length(I,tour);
  vector<sa_move<array_tour>> moves;
  // 2-opt
  struct {
    int i,j;
  } move2opt;
  moves.pb(sa_move<array_tour>
           { 1,
               ([&](array_tour const&) {
                 do {
                   move2opt.i=random(n-1);
                   move2opt.j=random(n-1);
                 } while(move2opt.i==move2opt.j);
                 if(move2opt.i>move2opt.j) swap(move2opt.i,move2opt.j);
               }),
               ([&](array_tour const& ar) -> double {
                 int i=move2opt.i;
                 int j=move2opt.j;
                 double delta = 0;
                 if(i == 0) {
                   delta += I.dist(0, ar.tour[i]);
                   delta -= I.dist(0, ar.tour[j]);
                 } else {
                   delta += I.dist(ar.tour[i-1], ar.tour[i]);
                   delta -= I.dist(ar.tour[i-1], ar.tour[j]);
                 }
                 if(j == n-2) {
                   delta += I.dist(0, ar.tour[j]);
                   delta -= I.dist(0, ar.tour[i]);
                 } else {
                   delta += I.dist(ar.tour[j], ar.tour[j+1]);
                   delta -= I.dist(ar.tour[i], ar.tour[j+1]);
                 }
                 return -delta;
               }),
               ([&](array_tour& ar) {
                 int i=move2opt.i;
                 int j=move2opt.j;
                 reverse(ar.tour.data()+i, ar.tour.data()+j+1);
               })
               });
  simulated_annealing<array_tour>
    (S, score,
     MAX_SA_TIME1, sqrt(score), // other max temperature ?
     (1<<12), (1<<22),
     moves,
     [&](double, double sc, array_tour const& S_) {
      score = sc;
      S = S_;
    });
  return S;
}

ttour greedy_packing(instance const& I, vi tour) {
  int n = I.n, m = I.m;
  ttour S(I, tour);

  // distance from a city to the end
  vector<double> D(n);
  D[0] = 0; D[tour[n-2]] = I.dist(0,tour[n-2]);
  FORD(i,n-3,0) D[tour[i]] = D[tour[i+1]] + I.dist(tour[i],tour[i+1]);

  auto itemValue = [&](int i) -> double {
    return (double) I.items[i].p / (double) I.items[i].w / (double) D[I.items[i].node];
  };

  vi J(m); iota(all(J), 0);
  sort(all(J), [&](int i, int j) {
      return itemValue(i) > itemValue(j);
    });

  double score = S.score(I);
  bool imp=0;
  auto add = [&]() -> bool {
    if(S.totalWeight > I.W) return 0;
    double score2 = S.score(I);
    if(score2 >= score) {
      imp=1;
      score = score2;
      return 1;
    }else{
      return 0;
    }
  };

  while(1) {
    imp=0;
    for(int i : J) {
      if(!S.packing[i]) { S.pack(I,i); if(!add()) S.unpack(I,i); }
      else { S.unpack(I,i); if(!add()) S.pack(I,i); }
    }
    cout << score << endl;
    if(!imp) break;
  }

  cout << score << " " << S.totalProfit << " " << S.totalWeight << endl;

  return S;
}

ttour sa_packing(instance const& I, ttour tour) {
  int n = I.n, m = I.m;
  (void) n;
  ttour S = tour;
  double score = S.score(I);
  vector<sa_move<ttour>> moves;
  // 2-opt
  struct {
    int i;
  } moveSwap;
  moves.pb(sa_move<ttour>
           { 1,
               ([&](ttour const&) {
                 moveSwap.i=random(m);
               }),
               ([&](ttour const& S2) -> double {
                 ttour S = S2;
                 double sscore = S.score(I);
                 int i = moveSwap.i;
                 if(!S.packing[i]) S.pack(I,i); else S.unpack(I,i);
                 bool ok = S.totalWeight <= I.W;
                 double nscore = S.score(I);
                 return ok?sscore-nscore:1e20;
               }),
               ([&](ttour& S) {
                 int i = moveSwap.i;
                 if(!S.packing[i]) S.pack(I,i); else S.unpack(I,i);
               })
               });
  simulated_annealing<ttour>
    (S, -score,
     MAX_SA_TIME1, sqrt(score), // other max temperature ?
     (1<<12), (1<<18),
     moves,
     [&](double, double sc, ttour const& S_) {
      score = -sc;
      S = S_;
    });
  cout << S.score(I) << " " << S.totalProfit << " " << S.totalWeight << endl;
  return S;
}

void dosmth(instance const& I, vi tour, ttour const& init) {
  int n = I.n, m = I.m;
  (void) m;
  vector<vector<tpl<double, double> > > V(n);
  vi W(n,0);
  FOR(i,n) {
    int is = I.cities[i].items.size();
    vector<tpl<double, double> > V2;
    FOR(mask, 1<<is) {
      double w=0;
      double p=0;
      FOR(j,is) if(mask & (1<<j)) {
        w += I.items[I.cities[i].items[j]].w;
        p += I.items[I.cities[i].items[j]].p;
      }
      V2.pb(mt(w,p));
    }
    sort(all(V2));
    double maxP=-1;
    for(auto p : V2) {
      if(p.y() > maxP) {
        V[i].pb(p);
        maxP = p.y();
        //if(init.cityWeights[i] >= p.x()-EPS) W[i] = V[i].size()-1;
      }
    }
  }

  ttour S(I, tour);
  FOR(i,n) S.setCity(i,V[i][W[i]]);
  double score = S.score(I);
  cout << score << endl;
  vector<sa_move<ttour>> moves;
  // move
  struct {
    int i;
    int nn;
  } movePack;
  moves.pb(sa_move<ttour>
           { 1,
               ([&](ttour const& S) {
                 while(1) {
                   movePack.i = random(n);
                   movePack.nn = random(V[movePack.i].size());
                   if(S.totalWeight + V[movePack.i][movePack.nn].x() - V[movePack.i][W[movePack.i]].x() > I.W) continue;
                   break;
                 }
               }),
               ([&](ttour& S) -> double {
                 double s0 = S.score(I);
                 S.setCity(movePack.i, V[movePack.i][movePack.nn]);
                 double s1 = S.score(I);
                 S.setCity(movePack.i, V[movePack.i][W[movePack.i]]);
                 return s0-s1;
               }),
               ([&](ttour& S) {
                 W[movePack.i] = movePack.nn;
                 S.setCity(movePack.i, V[movePack.i][W[movePack.i]]);
               })
               });
  cout << score << endl;
  simulated_annealing<ttour>
    (S, -score,
     MAX_SA_TIME2, 2*sqrt(abs(score)), // other max temperature ?
     (1<<12), (1<<18),
     moves,
     [&](double, double sc, ttour const& S_) {
      score = -sc;
      S = S_;
    });
  cout << S.score(I) << " " << S.totalProfit << " " << S.totalWeight << endl;


}

int main(int argc, char** argv){
  random_reset(time(0));

  assert(argc >= 2);
  char *filename = argv[1];

  double time_0 = timeInfo.getTime();

  instance I;

  { ifstream F(filename);
    I.read(F);
    if(!I.validateAndInit()) {
      throw runtime_error("Bad instance file...");
    }
  }
  I.print();

  vi tour = initial_tour(I);
  // random_shuffle(all(tour),[](int m) { return random(m); });
  cout << "Length: " << tour_length(I, tour) << endl;

  array_tour tour2 = tsp(I,tour);
  cout << tour_length(I,tour2.tour) << endl;
  auto ttour = greedy_packing(I,tour2.tour);
  dosmth(I, tour2.tour, ttour);
  reverse(all(tour2.tour));
  auto ttour2 = greedy_packing(I,tour2.tour);
  dosmth(I, tour2.tour, ttour2);

  // sa_packing(I,ttour);

  cerr << "Elapsed: " << (timeInfo.getTime()-time_0) << "s" << endl;

  return 0;
}
