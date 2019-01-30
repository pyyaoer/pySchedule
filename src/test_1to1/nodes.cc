#include "nodes.h"

BOOST_CLASS_EXPORT(BasicMessage)

void Client::HandleMessage(std::shared_ptr<Message> msg) {
  std::cout << "Round " << cnt++ << " Client" << std::endl;
  std::ostringstream archive_stream;
  boost::archive::text_oarchive archive(archive_stream);
  archive << msg;
  std::cout << "Receive message" << std::endl;
  msg->PrintMessage();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  std::shared_ptr<Message> new_msg = std::make_shared<BasicMessage>(port_, msg->GetSrcPort());
  BasicData d = { .data = "Client", };
  new_msg->SetData(d);
  std::cout << "Send message" << std::endl;
  new_msg->PrintMessage();
  out_msg_.push(new_msg);
}

void Server::HandleMessage(std::shared_ptr<Message> msg) {
  std::cout << "Round " << cnt++ << " Server" << std::endl;
  std::ostringstream archive_stream;
  boost::archive::text_oarchive archive(archive_stream);
  archive << msg;
  std::cout << "Receive message" << std::endl;
  msg->PrintMessage();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  std::shared_ptr<Message> new_msg = std::make_shared<BasicMessage>(port_, msg->GetSrcPort());
  BasicData d = { .data = "Server", };
  new_msg->SetData(d);
  std::cout << "Send message" << std::endl;
  new_msg->PrintMessage();
  out_msg_.push(new_msg);
}

