// -*- compile-command: "make all"; -*-
#include "header.h"
#include "instance.h"

const double MAX_TIME1 = 20.0;
const double MAX_TIME2 = 80.0;

partial_solution sa_1() {
  int n = I.n;
  solution S; double curScore = S.tourLength();
  partial_solution bestS = S; double bestScore = curScore;
  int niter=0;
  int lImp=-100;

  //

  double saStart = timeInfo.getTime();
  double maxTemp = 400.0;
  double temp = 0.0;
  //
  auto trySwap = [&](){
    int i=0, j=0;
    while(i==j) { i = random(n-1); j = random(n-1); }
    if(i>j) swap(i,j);
    double delta = 0;
    if(i == 0) {
      delta += I.dist(0, S.A[i]);
      delta -= I.dist(0, S.A[j]);
    } else {
      delta += I.dist(S.A[i-1], S.A[i]);
      delta -= I.dist(S.A[i-1], S.A[j]);
    }
    if(j == n-2) {
      delta += I.dist(0, S.A[j]);
      delta -= I.dist(0, S.A[i]);
    } else {
      delta += I.dist(S.A[j], S.A[j+1]);
      delta -= I.dist(S.A[i], S.A[j+1]);
    }
    if(delta >= - randomDouble() * temp) {
      reverse(S.A.data()+i, S.A.data()+j+1);
      curScore = curScore + delta;
      if(curScore > bestScore) {
          bestS = S;
          bestScore = curScore;
          if(lImp < niter-(1<<18)) {
            cerr << "Improve: " << bestScore << " " << S.TW << "/" << I.W << endl;
            lImp = niter;
          }
        }
    }
  };
  //
  while(1) {
    if((niter&((1<<20)-1)) == 0) {
      cout << niter << " " << (timeInfo.getTime() - saStart) << " " << curScore << " " << temp << endl;
    }
    if((niter&((1<<14)-1)) == 0) {
      double done = (timeInfo.getTime() - saStart) / MAX_TIME1;
      temp = (1.0 - pow(done, 0.15)) * maxTemp;
      if(done >= 1.0) break;
    }
    niter += 1;
    trySwap();
  }
  return bestS;
}

partial_solution greedy_2(partial_solution* initialS = 0) {
  int n = I.n, m = I.m;
  solution S;
  if(initialS) S.from_partial(*initialS);
  double curScore = S.score();

  vector<double> D(n);
  D[0] = 0; D[S.A.back()] = I.dist(0,S.A.back());
  FORD(i,n-3,0) D[S.A[i]] = D[S.A[i+1]] + I.dist(S.A[i],S.A[i+1]);
  vi J(m); iota(all(J), 0);
  auto itemValue = [&](int i) -> double {
    return (double) I.items[i].p / ((double) I.items[i].w * (double) D[I.items[i].node]);
  };
  sort(all(J), [&](int i, int j) {
      return itemValue(i) > itemValue(j);
    });

  auto add = [&]() -> bool {
    if(S.TW > I.W) return 0;
    double score = S.score();
    if(score >= curScore) {
      curScore = score;
      return 1;
    }else{
      return 0;
    }
  };

  FOR(k,16) for(int i : J) { S.flipB(i); if(!add()) S.flipB(i); }
  return S;
}

partial_solution sa_2(partial_solution* initialS = 0) {
  int n = I.n, m = I.m;
  solution S;
  if(initialS) S.from_partial(*initialS);
  double curScore = S.score();
  partial_solution bestS = S; double bestScore = curScore;
  int niter=0;
  int lImp=-100;

  //

  double saStart = timeInfo.getTime();
  double maxTemp = 1000.0;
  double temp = 0.0;

  auto add = [&]() -> bool {
    if(S.TW > I.W) return 0;
    double score = S.score();
    double delta = score - curScore;
    if(delta >= - randomDouble() * temp) {
      curScore = score;
      if(curScore > bestScore) {
        bestS = S;
        bestScore = curScore;
        if(lImp < niter-(1<<11)) {
          cerr << "Improve: " << bestScore << " " << S.TW << "/" << I.W << endl;
          lImp = niter;
        }
      }
      return 1;
    }else{
      return 0;
    }
  };
  //
  auto trySwap = [&](){
    int i=0, j=0;
    while(i==j) { i = random(n-1); j = random(n-1); }
    if(i>j) swap(i,j);
    reverse(S.A.data()+i, S.A.data()+j+1);
    if(!add()) {
      reverse(S.A.data()+i, S.A.data()+j+1);
    }
  };
  auto trySwap2 = [&](){
    int i=0, j=0;
    while(i==j) { i = random(m); j = random(m); }
    if(i>j) swap(i,j);
    S.flipB(i); S.flipB(j);
    if(!add()) {
      S.flipB(i); S.flipB(j);
    }
  };
  auto tryFlip = [&](){
    int i = random(m);
    S.flipB(i);
    if(!add()) {
      S.flipB(i);
    }
  };
  //
  while(1) {
    if((niter&131071) == 0) {
      cout << niter << " " << (timeInfo.getTime() - saStart) << " " << curScore << " " << temp << endl;
    }
    if((niter&2047) == 0) {
      double done = (timeInfo.getTime() - saStart) / MAX_TIME2;
      temp = (1.0 - pow(done, 0.15)) * maxTemp;
      if(done >= 1.0) break;
    }
    niter += 1;
    int r = random(5);
    if(r<=0) {
      trySwap();
    }else if(r<=1){
      trySwap2();
    }else{
      tryFlip();
    }
  }
  return bestS;
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

  partial_solution S1 = sa_1();
  cerr << "done S1" << endl;
  partial_solution G2 = greedy_2(&S1);
  cerr << "done G2" << endl;
  partial_solution S2 = sa_2(&G2);

  cerr << "Elapsed: " << (timeInfo.getTime()-time_0) << "s" << endl;

  return 0;
}
