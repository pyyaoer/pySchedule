#ifndef PYSCHEDULE_NODE_TEST_H_
#define PYSCHEDULE_NODE_TEST_H_

#include "include/node.h"
#include "basic_message.h"

#define TESTNODE_CLIENT_ID 1
#define TESTNODE_SERVER_ID 2

class Client : public Node {
 public:
  explicit Client(int node_id, boost::asio::io_service& service)
    : Node(node_id, service), cnt(0) {
    std::shared_ptr<Message> init_msg = std::make_shared<BasicMessage>(port_, TESTNODE_SERVER_ID + PORT_BASE);
    AtomicPushOutMessage(init_msg);
  }

 private:
  void HandleMessage(std::shared_ptr<Message> msg);

  int cnt;
};

class Server : public Node {
 public:
  explicit Server(int node_id, boost::asio::io_service& service)
    : Node(node_id, service), cnt(0) {}

 private:
  void HandleMessage(std::shared_ptr<Message> msg);

  int cnt;
};

#endif // PYSCHEDULE_NODE_TEST_H_
