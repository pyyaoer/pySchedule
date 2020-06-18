#ifndef PYSCHEDULE_ALGO_UNODES_H_
#define PYSCHEDULE_ALGO_UNODES_H_

#include "include/node.h"
#include "include/benchmark.h"
#include "umacros.h"
#include "umessages.h"

class User : public Node {
 public:
  explicit User(int tenant_id, int node_id, int msg_latency, boost::asio::io_service& service,
      int shape, double arg1, double arg2)
    : Node(node_id, msg_latency, service), tenant_id_(tenant_id), latency_summary_(0),
    counter_(0), bcmk_(shape, arg1, arg2) {}
  void Run();

 private:
  void HandleMessage(std::shared_ptr<Message> msg) {
    switch (msg->GetType()) {
      HandleMessageCase(Complete, msg);
      HandleMessageDefault;
    }
  }
  void HandleMessage_Complete(std::shared_ptr<CompleteMessage> msg);

  int tenant_id_;

  long long latency_summary_;
  long long maximum_latency_;
  int counter_;
  int msg_id_;
  SafeMap<int, long long> req_send_time_;

  Benchmark bcmk_;

  DISALLOW_COPY_AND_ASSIGN(User);
};

typedef struct {
  long long create_time;
  long long finish_time;
} HistoryItem;


#endif // PYSCHEDULE_ALGO_UNODES_H_

