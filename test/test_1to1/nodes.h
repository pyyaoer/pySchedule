#ifndef PYSCHEDULE_NODES_H_
#define PYSCHEDULE_NODES_H_

#include "include/node.h"
#include "messages.h"
#include "macros.h"

class Client : public Node {
 public:
  explicit Client(int node_id, boost::asio::io_service& service)
    : Node(node_id, service), cnt(0) {
    std::shared_ptr<Message> init_msg = std::make_shared<BasicMessage>(port_, SERVER_PORT);
    out_msg_.push(init_msg);
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

#endif // PYSCHEDULE_NODES_H_
