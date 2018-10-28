#include "include/node.h"

int main(void) {
  int node_id = 1;
  boost::asio::io_service io_service;
  Node n(node_id, io_service);
  boost::thread t(boost::bind(&Node::Run, &n));
  t.join();
}
