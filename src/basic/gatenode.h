#ifndef PYSCHEDULE_GATENODE_H_
#define PYSCHEDULE_GATENODE_H_

#include "include/node.h"
#include "src/basic/basic.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

class GateNode : public Node {

 public:
  explicit GateNode(int node_id, boost::asio::io_service& service)
    : Node(node_id, service) {}

 private:
  void HandleMessage(std::shared_ptr<Message> msg);
  void HandleMessage_Request(std::shared_ptr<RequestMessage> msg);

  DISALLOW_COPY_AND_ASSIGN(GateNode);
};

#endif // PYSCHEDULE_GATENODE_H_
