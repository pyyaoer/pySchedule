#include "include/boost_includes.h"
#include "include/host.h"

int main(void) {
  int host_id = 1;
  Host h(host_id);
  boost::thread t(boost::bind(&Host::Run, &h));
  t.join();
}
