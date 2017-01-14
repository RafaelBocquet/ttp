// -*- compile-command: "make main"; -*-
#pragma once
// #include "header.h" -- precompiled

// TODO: splay tree / treap / two level tree
// array representation of tours
struct array_tour {
  array_tour(vi const& tour_) : tour(tour_) {
  }

  vi tour;
};
