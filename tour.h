// -*- compile-command: "make main"; -*-
#pragma once
#include "instance.h"
// #include "header.h" -- precompiled

// TODO: splay tree / treap / two level tree
// array representation of tours
struct array_tour {
  array_tour(vi const& tour_) : tour(tour_) {
  }

  void swap2(int i, int j) {
    reverse(tour.data()+i, tour.data()+j+1);
  }

  vi tour;
};

// array representation of tours, with weights
struct array_tour2 {
  array_tour2(vector<double> const& speeds_, vi const& tour_) : speeds(speeds_), tour(tour_) {
    assert(speeds.size() == tour.size()+1);
  }

  double score(instance const& I) {
    int n = tour.size()+1;
    double sc = 0;
    int last = 0;
    FOR(i, n-1) {
      sc += speeds[i] * I.dist(last,tour[i]);
      last = tour[i];
    }
    sc += speeds[n-1] * I.dist(last,0);
    return sc;
  }

  void swap2(int i, int j) {
    reverse(tour.data()+i, tour.data()+j+1);
  }

  vector<double> speeds;
  vi tour;
};
