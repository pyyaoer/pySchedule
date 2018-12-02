#include <random>
#include "nodes.h"

BOOST_CLASS_EXPORT(RequestMessage)
BOOST_CLASS_EXPORT(CompleteMessage)
BOOST_CLASS_EXPORT(ActiveMessage)

void PNode::HandleMessage_Request(std::shared_ptr<RequestMessage> msg) {
}

void Gate::HandleMessage_Request(std::shared_ptr<RequestMessage> msg) {
  // Push the new request from User(tenent t) to the requests_[t] queue
  auto request = std::make_shared<RequestData>();
  msg->GetData(*request);
  requests_[request->tenent]->push(request);
  // Send a RequestMessage to PNode
  std::shared_ptr<Message> new_msg = std::make_shared<RequestMessage>(port_, GET_PORT(PNODE_ID_START));
  new_msg->SetData(*request);
  out_msg_.push(new_msg);
}

void Gate::HandleMessage_Active(std::shared_ptr<ActiveMessage> msg) {
}

void User::HandleMessage_Complete(std::shared_ptr<CompleteMessage> msg) {
}

void User::Run() {
  thread_pool_.emplace_back( [=]{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rand_gate(GATE_ID_START, GATE_ID_END-1);
    std::normal_distribution<> rand_time{1000, 100};
    std::normal_distribution<> rand_hardness{100, 10};

    while(true) {
      int gate_id = rand_gate(gen);
      int time_interval = rand_time(gen);
      int hardness_val = rand_hardness(gen);
      boost::this_thread::sleep_for(boost::chrono::milliseconds(time_interval));
      std::shared_ptr<Message> msg = std::make_shared<RequestMessage>(port_, GET_PORT(gate_id));
      RequestData r = {
        .tenent = tenent_id_,
	.gate = gate_id,
	.hardness = hardness_val,
      };
      msg->SetData(r);
      out_msg_.push(msg);
    }
  });
  Node::Run();
}
