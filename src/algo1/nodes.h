#ifndef PYSCHEDULE_ALGO1_NODES_H_
#define PYSCHEDULE_ALGO1_NODES_H_

#include "include/node.h"
#include "messages.h"
#include "macros.h"

class PNode : public Node {
 public:
  explicit PNode(int node_id, boost::asio::io_service& service)
    : Node(node_id, service) {}

 private:
  void HandleMessage(std::shared_ptr<Message> msg) {
    switch (msg->GetType()) {
      HandleMessageCase(Request, msg);
      HandleMessageDefault;
    }
  }
  void HandleMessage_Request(std::shared_ptr<RequestMessage> msg);

  DISALLOW_COPY_AND_ASSIGN(PNode);
};

class Gate : public Node {

 public:
  explicit Gate(int node_id, boost::asio::io_service& service)
    : Node(node_id, service) {}

 private:
  SafeQueue<std::shared_ptr<RequestData>> requests_;

  void HandleMessage(std::shared_ptr<Message> msg) {
    switch (msg->GetType()) {
      HandleMessageCase(Request, msg);
      HandleMessageCase(Active, msg);
      HandleMessageDefault;
    }
  }
  void HandleMessage_Request(std::shared_ptr<RequestMessage> msg);
  void HandleMessage_Active(std::shared_ptr<ActiveMessage> msg);

  DISALLOW_COPY_AND_ASSIGN(Gate);
};

class User : public Node {
 public:
  explicit User(int tenent_id, int node_id, boost::asio::io_service& service)
    : Node(node_id, service), tenent_id_(tenent_id) {}
  // A function should be defined here for statistics
  void GetStatistics();
  void Run();

 private:
  void HandleMessage(std::shared_ptr<Message> msg) {
    switch (msg->GetType()) {
      HandleMessageCase(Complete, msg);
      HandleMessageDefault;
    }
  }
  void HandleMessage_Complete(std::shared_ptr<CompleteMessage> msg);

  int tenent_id_;
  std::vector<time_t> req_send_time_;

  DISALLOW_COPY_AND_ASSIGN(User);
};

#endif // PYSCHEDULE_ALGO1_NODES_H_
