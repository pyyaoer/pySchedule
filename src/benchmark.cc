#include "include/benchmark.h"

const double pi = std::acos(-1);

// arg1: ratio (arg1 per second)
// e.g. 100
int Benchmark::StableNext() {
  std::normal_distribution<> rand_interval{1000/arg1, var};
  return rand_interval(gen);
}

// arg1: N per second
// arg2: small ratio
// half period: 10s
// e.g. N=10, small=5, large=2N-5=15
// status: 0 for large and 1 for small
int Benchmark::StepNext() {
  if (quota < 0) {
    status = -status;
    quota += 10000;
  }
  double ratio = (status > 0) ? arg1*2-arg2 : arg2;
  std::normal_distribution<> rand_interval{1000/ratio, var};
  int ret = rand_interval(gen);
  quota -= ret;
  return ret;
}

// arg1: N' per second
// arg2: period T
// N'=N(1+1/pi)
// ratio = N(sin(2pi*t/T)+1)
// e.g. T=20s, N'=10
// status: t
int Benchmark::SineNext() {
  int delta = 1000;
  double T = arg2;
  double N = arg1/(1+1/pi)/1000;
  while (quota < 0) {
    status += delta;
    quota += N*delta + N*T * std::sin(delta*pi/T)*std::sin(2*pi*status/T-delta*pi/T) / pi;
    if (status > T)
      status -= T;
  }
  double ratio = N*(std::sin(2*pi*status/T)+1);
  if (ratio < 1/5000.0) ratio = 1/5000.0;
  std::normal_distribution<> rand_interval{1/ratio, var};
  int ret = std::round(rand_interval(gen));
  quota -= ret;
  return ret;
}

int Benchmark::BurstyNext() {
  return 0;
}

int Benchmark::next() {
  int ret = -1;
  switch(shape) {
    case 0:
      ret = StableNext(); break;
    case 1:
      ret = StepNext(); break;
    case 2:
      ret = SineNext(); break;
    case 3:
      ret = BurstyNext(); break;
  }
  if (ret < 0) ret = 1;
  return ret;
}


