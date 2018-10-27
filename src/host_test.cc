#include "include/boost_includes.h"
#include "include/host.h"

int main(void) {
  int host_id = 1;
  boost::asio::io_service io_service;
  Host h(host_id, io_service);
  boost::thread t(boost::bind(&Host::Run, &h));
  t.join();
}
