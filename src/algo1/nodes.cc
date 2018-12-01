#include "nodes.h"

BOOST_CLASS_EXPORT(RequestMessage)
BOOST_CLASS_EXPORT(CompleteMessage)
BOOST_CLASS_EXPORT(ActiveMessage)

/*
void User::HandleMessage(std::shared_ptr<Message> msg) {
  std::cout << "Round " << cnt++ << " Client" << node_id_ << std::endl;
  std::ostringstream archive_stream;
  boost::archive::text_oarchive archive(archive_stream);
  archive << msg;
  std::cout << "Receive message" << std::endl;
  msg->PrintMessage();

  std::shared_ptr<Message> new_msg = std::make_shared<BasicMessage>(port_, msg->GetSrcPort());
  BasicData d = { .data = "Client", };
  new_msg->SetData(d);
  std::cout << "Send message" << std::endl;
  new_msg->PrintMessage();
  AtomicPushOutMessage(new_msg);
}
*/

void PNode::HandleMessage_Request(std::shared_ptr<RequestMessage> msg) {
}

void Gate::HandleMessage_Request(std::shared_ptr<RequestMessage> msg) {
}

void Gate::HandleMessage_Active(std::shared_ptr<ActiveMessage> msg) {
}

void User::HandleMessage_Complete(std::shared_ptr<CompleteMessage> msg) {
}


