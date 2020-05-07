#ifndef PYSCHEDULE_BENCHMARK_H_
#define PYSCHEDULE_BENCHMARK_H_

#include "lib_include.h"

#include <cmath>
#include <random>

class Benchmark {
 public:
  Benchmark() {}
  explicit Benchmark(int shape, double arg1, double arg2):
    shape(shape), arg1(arg1), arg2(arg2), gen(rd()), var(10) {
      switch (shape) {
	case 0:break;
	case 1:
	       quota=1000;
	       status=1;
	       break;
	case 2:
	       quota=-1;
	       status=-1000;
	       break;
	case 3:
	       break;
      }
    }
  virtual ~Benchmark() = default;

  int next();

 private:
  // shape: 0, 1, 2, 3
  // 0: stable
  // 1: step
  // 2: sine
  // 3: bursty
  int shape;
  double arg1;
  double arg2;
  std::random_device rd;
  std::mt19937 gen;

  double var;
  double quota;
  double status;

  int StableNext();
  int StepNext();
  int SineNext();
  int BurstyNext();

  DISALLOW_COPY_AND_ASSIGN(Benchmark);
};

#endif // PYSCHEDULE_BENCHMARK_H_
