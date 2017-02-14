// -*- compile-command: "make main"; -*-
#pragma once
// #include "header.h" -- precompiled

// tour with packing data, and fixed city tour
struct ttour {
  ttour(instance const& I, vi const& tour_) {
    n=I.n; m=I.m;
    tour = tour_;
    assert((int)tour.size() == n-1);
    cityWeights.assign(n,0);
    cityProfits.assign(n,0);
    tourDist.assign(n,0);
    { int last=0;
      FOR(i,n-1) {
        tourDist[i] = I.dist(last,tour[i]);
        last = tour[i];
      }
      tourDist[n-1] = I.dist(last,0);
    }
    packing.assign(m,0);
  }

  int n, m;
  vi tour;
  vector<double> tourDist;
  double totalProfit = 0;
  double totalWeight = 0;
  vector<double> cityWeights;
  vector<double> cityProfits;

  vector<bool> packing;

  double score(instance const& I) const {
    double sc = totalProfit;
    double weight = 0;
    FOR(i,n-1) {
      sc -= tourDist[i] * I.R / (I.vmax - I.nu * weight);
      weight += cityWeights[tour[i]];
    }
    sc -= I.R * tourDist[n-1] / (I.vmax - I.nu * weight);
    return sc;
  }

  void pack(instance const& I, int i) {
    assert(!packing[i]);
    packing[i] = 1;
    totalProfit += I.items[i].p;
    totalWeight += I.items[i].w;
    cityWeights[I.items[i].node] += I.items[i].w;
    cityProfits[I.items[i].node] += I.items[i].p;
  }

  void unpack(instance const& I, int i) {
    assert(packing[i]);
    packing[i] = 0;
    totalProfit -= I.items[i].p;
    totalWeight -= I.items[i].w;
    cityWeights[I.items[i].node] -= I.items[i].w;
    cityProfits[I.items[i].node] -= I.items[i].p;
  }

  void setCity(int i, double w, double p) {
    totalProfit += p-cityProfits[i];
    totalWeight += w-cityWeights[i];
    cityWeights[i]=w;
    cityProfits[i]=p;
  }

  void setCity(int i, tpl<double, double> v) {
    setCity(i,v.x(),v.y());
  }
};
