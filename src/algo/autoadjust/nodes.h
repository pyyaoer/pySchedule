#ifndef PYSCHEDULE_ALGO_AUTOADJUST_NODES_H_
#define PYSCHEDULE_ALGO_AUTOADJUST_NODES_H_

#include "include/node.h"
#include "messages.h"
#include "macros.h"
#include "../unodes.h"

class PNode : public Node {
  class Records {
    class Record {
      public:
      long long start;
      long long end;
      int cnt;
      Record() : Record(0, 0, -1) {}
      explicit Record(long long s, long long e, int c)
        : start(s), end(e), cnt(c) {}
    };
    public:
    std::vector<Record> records;
    explicit Records() : records(GATE_NUM) {}
    int UpdateAndCount(int gate, int cnt, long long start, long long end) {
      records[gate] = Record(start, end, cnt);
      int s = 0;
      for (auto r : records) {
        if (r.cnt < 0) continue;
        if (start <= r.start and r.end <= end) {
          s += r.cnt;
        }
        else if (start <= r.end and r.end <= end) {
          // Assume that the requests follow a uniform distribution
          s += r.cnt * (r.end-start) / (r.end-r.start);
        }
      }
      return s;
    }
  };

 public:
  explicit PNode(int node_id, int msg_latency, boost::asio::io_service& service)
    : Node(node_id, msg_latency, service),
      record_r_(TENANT_NUM), record_d_(TENANT_NUM) {}
  void Run();

 private:
  std::vector<Records> record_r_;
  std::vector<Records> record_d_;

  void HandleMessage(std::shared_ptr<Message> msg) {
    switch (msg->GetType()) {
      HandleMessageCase(Sync, msg);
      HandleMessageDefault;
    }
  }
  void HandleMessage_Sync(std::shared_ptr<SyncMessage> msg);

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
  explicit Gate(int node_id, int msg_latency, int period, boost::asio::io_service& service)
    : Node(node_id, msg_latency, service), period_(period), request_id_(0), parameter_mutex_(),
    rtag_(TENANT_NUM, 0), ltag_(TENANT_NUM, 0), ptag_(TENANT_NUM, 0),
    rho_(TENANT_NUM, ((double)GATE_NUM) / period_), delta_(TENANT_NUM, ((double)GATE_NUM)/period_),
    last_request_time_(TENANT_NUM, 0) {
    todo_list_ = std::make_shared<TodoList>();
    idle_slots_ = std::make_shared<IdleQueue>();
    for (int i = 0; i < THREADS_PER_GATE; i++) {
      idle_slots_->push(true);
    }
    for (int i = 0; i < TENANT_NUM; ++i) {
      scheduled_.push_back(std::vector<int>(2, 0));
    }
  }
  void Run();

 private:
  int period_;
  int request_id_;
  std::vector<double> rtag_;
  std::vector<double> ltag_;
  std::vector<double> ptag_;
  std::vector<double> rho_;
  std::vector<double> delta_;
  std::vector<long long> last_request_time_;
  mutable std::mutex parameter_mutex_;
  std::shared_ptr<TodoList> todo_list_;
  std::shared_ptr<IdleQueue> idle_slots_;
  std::vector<std::vector<int>> scheduled_;

  void HandleMessage(std::shared_ptr<Message> msg) {
    switch (msg->GetType()) {
      HandleMessageCase(Request, msg);
      HandleMessageCase(Tags, msg);
      HandleMessageDefault;
    }
  }
  void HandleMessage_Request(std::shared_ptr<RequestMessage> msg);
  void HandleMessage_Tags(std::shared_ptr<TagsMessage> msg);

  DISALLOW_COPY_AND_ASSIGN(Gate);
};

#endif // PYSCHEDULE_ALGO_AUTOADJUST_NODES_H_

