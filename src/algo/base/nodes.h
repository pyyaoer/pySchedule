#ifndef PYSCHEDULE_ALGO_BASE_NODES_H_
#define PYSCHEDULE_ALGO_BASE_NODES_H_

#include "include/node.h"
#include "messages.h"
#include "macros.h"
#include "../unodes.h"

class PNode : public Node {

 public:
  explicit PNode(int node_id, int msg_latency, boost::asio::io_service& service)
    : Node(node_id, msg_latency, service) {
    for (int i = 0; i < GATE_NUM; ++i) {
      rho_.push_back(std::vector<int>(TENANT_NUM, 1));
      delta_.push_back(std::vector<int>(TENANT_NUM, 1));
    }
  }
  void Run();

 private:
  std::vector<std::vector<int> > rho_;
  std::vector<std::vector<int> > delta_;

  void HandleMessage(std::shared_ptr<Message> msg) {
    switch (msg->GetType()) {
      HandleMessageCase(Request, msg);
      HandleMessageCase(Scheduled, msg);
      HandleMessageDefault;
    }
  }
  void HandleMessage_Request(std::shared_ptr<RequestMessage> msg);
  void HandleMessage_Scheduled(std::shared_ptr<ScheduledMessage> msg);

  DISALLOW_COPY_AND_ASSIGN(PNode);
};

class Gate : public Node {

  typedef struct {
    double r_tag;
    double l_tag;
    double p_tag;
    RequestData data;
  } TodoItem;
  using TodoList = SafeList<std::shared_ptr<TodoItem> >;
  using RequestList = SafeList<std::shared_ptr<RequestData> >;
  using IdleQueue = SafeQueue<bool>;

 public:
  explicit Gate(int node_id, int msg_latency, boost::asio::io_service& service)
    : Node(node_id, msg_latency, service), request_id_(0), tag_mutex_(),
    rtag_(TENANT_NUM, 0), ltag_(TENANT_NUM, 0), ptag_(TENANT_NUM, 0) {
    todo_list_ = std::make_shared<TodoList>();
    requests_ = std::make_shared<RequestList>();
    idle_slots_ = std::make_shared<IdleQueue>();
    for (int i = 0; i < THREADS_PER_GATE; i++) {
      idle_slots_->push(true);
    }
  }
  void Run();

 private:
  int request_id_;
  std::vector<double> rtag_;
  std::vector<double> ltag_;
  std::vector<double> ptag_;
  mutable std::mutex tag_mutex_;
  std::shared_ptr<TodoList> todo_list_;
  std::shared_ptr<RequestList> requests_;
  std::shared_ptr<IdleQueue> idle_slots_;

  void HandleMessage(std::shared_ptr<Message> msg) {
    switch (msg->GetType()) {
      HandleMessageCase(Request, msg);
      HandleMessageCase(Tag, msg);
      HandleMessageDefault;
    }
  }
  void HandleMessage_Request(std::shared_ptr<RequestMessage> msg);
  void HandleMessage_Tag(std::shared_ptr<TagMessage> msg);

  DISALLOW_COPY_AND_ASSIGN(Gate);
};

#endif // PYSCHEDULE_ALGO_BASE_NODES_H_

