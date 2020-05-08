#include <cstdlib>
#include <ctime>

#include "nodes.h"

int main(void) {
  boost::asio::io_service io_service;
  std::vector<std::shared_ptr<boost::thread>> thread_pool;

  for (int i = CLIENT_ID_START; i < CLIENT_ID_END; ++i) {
    auto client = std::make_shared<Client>(i, io_service);
    thread_pool.push_back(std::make_shared<boost::thread>(boost::bind(&Client::Run, client)));
  }
  auto server = std::make_shared<Server>(SERVER_ID, io_service);
    thread_pool.push_back(std::make_shared<boost::thread>(boost::bind(&Server::Run, server)));

  for (auto t : thread_pool) {
    t->join();
  }
}
