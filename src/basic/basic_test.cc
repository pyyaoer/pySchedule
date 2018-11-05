#include "include/node.h"
#include "src/basic/usernode.h"
#include "src/basic/gatenode.h"

int main(void) {
  boost::asio::io_service io_service;
  std::vector<std::shared_ptr<boost::thread>> thread_pool;

  auto gatenode = std::make_shared<GateNode>(GATE_ID_START, io_service);
  thread_pool.push_back(std::make_shared<boost::thread>(boost::bind(&GateNode::Run, gatenode)));

  for (int i = 0; i < USER_NUM; ++i) {
    auto usernode = std::make_shared<UserNode>(0, USER_ID_START + i, io_service);
    thread_pool.push_back(std::make_shared<boost::thread>(boost::bind(&UserNode::Run, usernode)));
  }
  for (auto t : thread_pool) {
    t->join();
  }
}
