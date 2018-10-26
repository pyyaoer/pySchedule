#ifndef PYSCHEDULE_HOST_H_
#define PYSCHEDULE_HOST_H_

#include <stdio.h>
#include <boost/thread.hpp>

#include "include/boost_includes.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

class Host {
 public:
  Host() { Host(-1); };
  explicit Host(int host): host_id_(host) {};
  ~Host() {};

  void Run() {
    while(1) {
      printf("Host %d is running!\n", host_id_);
      boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
  }

 private:

  int host_id_;

  DISALLOW_COPY_AND_ASSIGN(Host);
};

#endif // PYSCHEDULE_HOST_H_
