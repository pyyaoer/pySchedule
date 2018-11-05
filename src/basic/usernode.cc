#include "src/basic/usernode.h"
#include <random>
#include <time.h>

void UserNode::HandleMessage(std::shared_ptr<Message> msg) {
  switch(msg->GetType()) {
    case COMPLETE:
      HandleMessage_Complete(std::dynamic_pointer_cast<CompleteMessage>(msg));
    default:
      std::cout << "Undefined message type." << std::endl;
  }
}

void UserNode::HandleMessage_Complete(std::shared_ptr<CompleteMessage> msg) {
}

void UserNode::Run() {
  srand((unsigned)time(0));
  thread_pool_.emplace_back([=]{
    while(true) {
      // There is only one gate in the basic system 
      short gate_id = GATE_ID_START;
      short gate_port = gate_id + PORT_BASE;
      // Build an IO request
      auto new_msg = std::make_shared<RequestMessage>(port_, gate_port, 100);
      // Send the message
      AtomicPushOutMessage(new_msg);
      req_send_time_.push_back(time(0));
      // Sleep for a while
      boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    }
    Node::Run();
  });
}
