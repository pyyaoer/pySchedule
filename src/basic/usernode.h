#ifndef PYSCHEDULE_USERNODE_H_
#define PYSCHEDULE_USERNODE_H_

#include "include/node.h"
#include "src/basic/basic.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

class UserNode : public Node {

 public:
  explicit UserNode(int tenent_id, int node_id, boost::asio::io_service& service)
    : Node(node_id, service), tenent_id_(tenent_id) {}
  // A function should be defined here for statistics
  void GetStatistics();
  void Run();

 private:
  void HandleMessage(std::shared_ptr<Message> msg);
  void HandleMessage_Complete(std::shared_ptr<CompleteMessage> msg);

  int tenent_id_;
  std::vector<time_t> req_send_time_;

  DISALLOW_COPY_AND_ASSIGN(UserNode);
};

#endif // PYSCHEDULE_USERNODE_H_
