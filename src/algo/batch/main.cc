#include <cstdlib>
#include <ctime>

#include "nodes.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

int PNODE_NUM_ = 1;
int GATE_NUM_ = 1;
int USER_NUM_ = 16;
int TENANT_NUM_ = 3;
int THREADS_PER_GATE_ = 500;
int TENANT_LIMIT_ = 100;
int TENANT_RESERVATION_ = 50;
int BENCHMARK_SHAPE_ = 0;
int MSG_LATENCY_ = 10;
int GATE_PERIOD_ = 1000;

int main(int argc, char **argv) {

  if (argc == 2)
  {
    std::ifstream in(argv[1]);
    boost::property_tree::ptree ptree;
    boost::property_tree::read_json(in, ptree);
    PNODE_NUM_ = ptree.get<int>("PNODE_NUM");
    GATE_NUM_ = ptree.get<int>("GATE_NUM");
    USER_NUM_ = ptree.get<int>("USER_NUM");
    TENANT_NUM_ = ptree.get<int>("TENANT_NUM");
    THREADS_PER_GATE_ = ptree.get<int>("THREADS_PER_GATE");
    TENANT_LIMIT_ = ptree.get<int>("TENANT_LIMIT");
    TENANT_RESERVATION_ = ptree.get<int>("TENANT_RESERVATION");
    BENCHMARK_SHAPE_ = ptree.get<int>("BENCHMARK_SHAPE");
    MSG_LATENCY_ = ptree.get<int>("MSG_LATENCY");
    GATE_PERIOD_ = ptree.get<int>("GATE_PERIOD");
  }
  else if (argc != 1)
  {
    std::cerr << "Bad argument!" << std::endl;
    return -1;
  }

  boost::asio::io_service io_service;
  std::vector<std::shared_ptr<boost::thread>> thread_pool;

  for (int i = PNODE_ID_START; i < PNODE_ID_END; ++i) {
    auto pnode = std::make_shared<PNode>(i, MSG_LATENCY_, io_service);
    thread_pool.push_back(std::make_shared<boost::thread>(boost::bind(&PNode::Run, pnode)));
  }
  for (int i = GATE_ID_START; i < GATE_ID_END; ++i) {
    auto gate = std::make_shared<Gate>(i, GATE_PERIOD_, MSG_LATENCY_, io_service);
    thread_pool.push_back(std::make_shared<boost::thread>(boost::bind(&Gate::Run, gate)));
  }
  for (int i = USER_ID_START; i < USER_ID_END; ++i) {
    std::shared_ptr<User> user;
    switch(BENCHMARK_SHAPE_) {
      case 0:
        user = std::make_shared<User>(i % TENANT_NUM, i, MSG_LATENCY_, io_service, 0, 10, 0);
	break;
      case 1:
        user = std::make_shared<User>(i % TENANT_NUM, i, MSG_LATENCY_, io_service, 1, 10, 5);
	break;
      case 2:
	user = std::make_shared<User>(i % TENANT_NUM, i, MSG_LATENCY_, io_service, 2, 10, 20000);
	break;
    }
    thread_pool.push_back(std::make_shared<boost::thread>(boost::bind(&User::Run, user)));
  }

  for (auto t : thread_pool) {
    t->join();
  }
}
