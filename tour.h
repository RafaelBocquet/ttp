// -*- compile-command: "make main"; -*-
#pragma once
// #include "header.h" -- precompiled

// array representation of tours
struct array_tour {
  array_tour(vi const& tour_) : tour(tour_) {
  }

  vi tour;
};
