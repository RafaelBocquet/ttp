// -*- compile-command: "make main"; -*-
#pragma once
// #include "header.h" -- precompiled

template<typename SOL>
struct sa_move {
  int weight;
  function<void(SOL const&)> pick_random;
  function<double(SOL&)> eval; // should not modify SOL
  function<void(SOL&)> apply;
};

double SA_PARAM = 0.15;

// minimization
template<typename SOL>
void simulated_annealing(SOL S, double score,
                         double availableTime,
                         double maxTemp,
                         int updateTempTime,
                         int printInfoTime,
                         vector<sa_move<SOL>> moves,
                         function<void(double, double, SOL const& s)> onImprovement) {
  assert(moves.size());
  long long int iterationCount = 0; // can get over 2^32
  int updateTempCounter = updateTempTime;
  int printInfoCounter = printInfoTime;

  double bestScore = score;
  double done=0;

  double saStart = timeInfo.getTime();
  double temp = maxTemp;

  int acCount = 0;

  vi  movePicker;
  FOR(i, moves.size()) {
    FOR(j,moves[i].weight) movePicker.pb(i);
  }

  while(1) {
    iterationCount += 1;
    if(!(--printInfoCounter)) {
      cout << iterationCount << " " << score << " " << bestScore << endl;
      printInfoCounter = printInfoTime;
    }
    if(!(--updateTempCounter)) {
      done = (timeInfo.getTime() - saStart) / availableTime;
      temp = (1.0 - pow(done, SA_PARAM)) * maxTemp;
      if(done >= 1.0) break;
      updateTempCounter = updateTempTime;
    }

    // try a move
    sa_move<SOL> const& m = moves[movePicker[random(movePicker.size())]];
    m.pick_random(S);
    double delta = m.eval(S);
    if(delta <= randomDouble() * temp) {
      acCount++;
      m.apply(S);
      score += delta;
      if(score < bestScore) {
        bestScore = score;
        onImprovement(done, bestScore, S);
      }
    }
  }
  cout << "Accepted: " << acCount << "/" << iterationCount << " : " << (double)acCount/(double)iterationCount << endl;
}
