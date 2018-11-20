#include "include/node.h"

#define TESTNODE_CLIENT_ID 1
#define TESTNODE_SERVER_ID 2

enum eMessageType{ BASIC = 0 };

class Client : public Node {
public:
  int cnt;
  explicit Client(int node_id, boost::asio::io_service& service)
    : Node(node_id, service) {
    cnt = 0;
    std::shared_ptr<Message> init_msg(
      new Message(port_, TESTNODE_SERVER_ID + PORT_BASE, BASIC)
    );
    AtomicPushOutMessage(init_msg);
  }
 private:
  void HandleMessage(std::shared_ptr<Message> msg) {
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << msg;
    std::cout << "Client: " << archive_stream.str() << " " << cnt++ << std::endl;
    //std::cout << "Client: " << msg->ToString() << " " << cnt++ << std::endl;
    auto new_msg = std::make_shared<Message>(port_, msg->GetSrcPort(), BASIC);
    AtomicPushOutMessage(new_msg);
  }
};

class Server : public Node {
 public:
  int cnt;
  explicit Server(int node_id, boost::asio::io_service& service)
    : Node(node_id, service) {
    cnt = 0;
  }
 private:
  void HandleMessage(std::shared_ptr<Message> msg) {
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << msg;
    std::cout << "Server: " << archive_stream.str() << " " << cnt++ << std::endl;
    //std::cout << "Server: " << msg->ToString() << " " << cnt++ << std::endl;
    auto new_msg = std::make_shared<Message>(port_, msg->GetSrcPort(), BASIC);
    AtomicPushOutMessage(new_msg);
  }
};

int main(void) {
  boost::asio::io_service io_client;
  boost::asio::io_service io_server;
  auto client = std::make_shared<Client>(TESTNODE_CLIENT_ID, io_client);
  auto server = std::make_shared<Server>(TESTNODE_SERVER_ID, io_server);
  boost::thread client_thread(boost::bind(&Client::Run, client));
  boost::thread server_thread(boost::bind(&Server::Run, server));
  client_thread.join();
  server_thread.join();
}
