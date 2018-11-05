#include "src/basic/gatenode.h"
#include <time.h>

void GateNode::HandleMessage(std::shared_ptr<Message> msg) {
  switch(msg->GetType()) {
    case REQUEST:
      HandleMessage_Request(std::dynamic_pointer_cast<RequestMessage>(msg));
    default:
      std::cout << "Undefined message type." << std::endl;
  }
}

void GateNode::HandleMessage_Request(std::shared_ptr<RequestMessage> msg) {
  std::cout << msg->ToString() << std::endl;
}

