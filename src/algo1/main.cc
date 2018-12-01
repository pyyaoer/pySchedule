#include <cstdlib>
#include <ctime>

#include "nodes.h"

int main(void) {
  boost::asio::io_service io_service;
  std::vector<std::shared_ptr<boost::thread>> thread_pool;

  for (int i = PNODE_ID_START; i < PNODE_ID_END; ++i) {
    auto pnode = std::make_shared<PNode>(i, io_service);
    thread_pool.push_back(std::make_shared<boost::thread>(boost::bind(&PNode::Run, pnode)));
  }
  for (int i = GATE_ID_START; i < GATE_ID_END; ++i) {
    auto gate = std::make_shared<Gate>(i, io_service);
    thread_pool.push_back(std::make_shared<boost::thread>(boost::bind(&Gate::Run, gate)));
  }
  for (int i = USER_ID_START; i < USER_ID_END; ++i) {
    auto user = std::make_shared<User>(i % TENENT_NUM, i, io_service);
    thread_pool.push_back(std::make_shared<boost::thread>(boost::bind(&User::Run, user)));
  }

  for (auto t : thread_pool) {
    t->join();
  }
}
