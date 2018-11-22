#include "include/node.h"
#include <cstdlib>
#include <ctime>

#define TESTNODE_CLIENT_ID 1
#define TESTNODE_SERVER_ID 2

enum eMessageType{ BASIC = 0 };

typedef struct {
  char data[7];
} BasicData;

// Message with at most 7 characters
class BasicMessage: public Message {
 public:
  BasicMessage() {}
  explicit BasicMessage(short src_port, short dst_port)
    : Message(src_port, dst_port, BASIC, sizeof(BasicData)) {}

  template <class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & boost::serialization::base_object<Message>(*this);
  }
};

BOOST_CLASS_EXPORT(BasicMessage)

class Client : public Node {
public:
  int cnt;
  explicit Client(int node_id, boost::asio::io_service& service)
    : Node(node_id, service) {
    cnt = 0;
    //std::shared_ptr<Message> init_msg = std::make_shared<BasicMessage>(port_, TESTNODE_SERVER_ID + PORT_BASE);
    std::shared_ptr<Message> init_msg(
      new BasicMessage(port_, TESTNODE_SERVER_ID + PORT_BASE)
      //new Message(port_, TESTNODE_SERVER_ID + PORT_BASE, BASIC, sizeof(BasicData))
    );
    AtomicPushOutMessage(init_msg);
  }
 private:
  void HandleMessage(std::shared_ptr<Message> msg) {

    std::cout << "Round " << cnt++ << " Client" << std::endl;
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << msg;
    std::cout << "Receive message" << std::endl;
    msg->PrintMessage();

    std::shared_ptr<Message> new_msg = std::make_shared<BasicMessage>(port_, msg->GetSrcPort());
    BasicData d = { .data = "Client", };
    new_msg->SetData(d);
    std::cout << "Send message" << std::endl;
    new_msg->PrintMessage();
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

    std::cout << "Round " << cnt++ << " Server" << std::endl;
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << msg;
    std::cout << "Receive message" << std::endl;
    msg->PrintMessage();

    std::shared_ptr<Message> new_msg = std::make_shared<BasicMessage>(port_, msg->GetSrcPort());
    BasicData d = { .data = "Server", };
    new_msg->SetData(d);
    std::cout << "Send message" << std::endl;
    new_msg->PrintMessage();
    AtomicPushOutMessage(new_msg);
  }
};

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
