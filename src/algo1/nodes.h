#ifndef PYSCHEDULE_ALGO1_NODES_H_
#define PYSCHEDULE_ALGO1_NODES_H_

#include "include/node.h"
#include "messages.h"
#include "macros.h"

class PNode : public Node {

  typedef struct {
    int gate;
    int tenent;
    long long time;
  } TodoItem;
  using TodoList = SafeList<std::shared_ptr<TodoItem> >;
  using DoneQueue = SafeQueue<long long>;
  using IdleQueue = SafeQueue<bool>;

 public:
  explicit PNode(int node_id, boost::asio::io_service& service, long long time_window)
    : Node(node_id, service), time_window_(time_window) {
    todo_list_ = std::make_shared<TodoList>();
    for (int i = 0; i < TENENT_NUM; ++i) {
      auto dq = std::make_shared<DoneQueue>();
      done_list_.push_back(dq);
    }
    for (int i = 0; i < GATE_NUM; ++i) {
      auto iq = std::make_shared<IdleQueue>();
      for (int j = 0; j < THREADS_PER_GATE; j++) {
        iq->push(true);
      }
      idle_list_.push_back(iq);
    }
  }
  void Run();

 private:
  long long time_window_;
  std::shared_ptr<TodoList> todo_list_;
  std::vector<std::shared_ptr<IdleQueue> > idle_list_;
  std::vector<std::shared_ptr<DoneQueue> > done_list_;

  void HandleMessage(std::shared_ptr<Message> msg) {
    switch (msg->GetType()) {
      HandleMessageCase(Request, msg);
      HandleMessageCase(Complete, msg);
      HandleMessageDefault;
    }
  }
  void HandleMessage_Request(std::shared_ptr<RequestMessage> msg);
  void HandleMessage_Complete(std::shared_ptr<CompleteMessage> msg);

  DISALLOW_COPY_AND_ASSIGN(PNode);
};

class Gate : public Node {

  using RequestQueue = SafeQueue<std::shared_ptr<RequestData> >;

 public:
  explicit Gate(int node_id, boost::asio::io_service& service)
    : Node(node_id, service) {
    for (int i = 0; i < TENENT_NUM; ++i) {
      auto rq = std::make_shared<RequestQueue>();
      requests_.push_back(rq);
    }
  }
  void Run();

 private:
  std::vector<std::shared_ptr<RequestQueue> > requests_;

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
    : Node(node_id, service), tenent_id_(tenent_id), latency_summary_(0), counter_(0) {}
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

  long long latency_summary_;
  long long maximum_latency_;
  int counter_;
  int msg_id_;
  SafeMap<int, long long> req_send_time_;

  DISALLOW_COPY_AND_ASSIGN(User);
};

#endif // PYSCHEDULE_ALGO1_NODES_H_
