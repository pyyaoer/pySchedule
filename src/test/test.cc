#include <cstdlib>
#include <ctime>

#include "node_test.h"

int main(void) {
  std::srand(std::time(nullptr));
  boost::asio::io_service io_client;
  boost::asio::io_service io_server;
  auto client = std::make_shared<Client>(TESTNODE_CLIENT_ID, io_client);
  auto server = std::make_shared<Server>(TESTNODE_SERVER_ID, io_server);
  boost::thread client_thread(boost::bind(&Client::Run, client));
  boost::thread server_thread(boost::bind(&Server::Run, server));
  client_thread.join();
  server_thread.join();
}
