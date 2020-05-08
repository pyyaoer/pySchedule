#include <cstdlib>
#include <ctime>

#include "nodes.h"

int main(void) {
  std::srand(std::time(nullptr));
  boost::asio::io_service io_client;
  boost::asio::io_service io_server;
  auto client = std::make_shared<Client>(CLIENT_ID, io_client);
  auto server = std::make_shared<Server>(SERVER_ID, io_server);
  boost::thread client_thread(boost::bind(&Client::Run, client));
  boost::thread server_thread(boost::bind(&Server::Run, server));
  client_thread.join();
  server_thread.join();
}
