#include <random>
#include "nodes.h"

BOOST_CLASS_EXPORT(RequestMessage)
BOOST_CLASS_EXPORT(CompleteMessage)
BOOST_CLASS_EXPORT(ActiveMessage)

void PNode::HandleMessage_Request(std::shared_ptr<RequestMessage> msg) {
  // Push the new request to the todo list
  RequestData r;
  msg->GetData(r);
  auto item = std::make_shared<TodoItem>(
    TodoItem {
      .tenent = r.tenent,
      .gate = r.gate,
      .time = msg->GetCreateTime(),
    }
  );
  todo_list_.push(item);
}

void PNode::HandleMessage_Complete(std::shared_ptr<CompleteMessage> msg) {
  CompleteData c;
  msg->GetData(c);
  done_list_[c.tenent]->push(msg->GetCreateTime());
}

void Gate::HandleMessage_Request(std::shared_ptr<RequestMessage> msg) {
  // Push the new request from User(tenent t) to the requests_[t] queue
  auto r = std::make_shared<RequestData>();
  msg->GetData(*r);
  requests_[r->tenent]->push(r);
  // Send a RequestMessage to PNode
  std::shared_ptr<Message> new_msg = std::make_shared<RequestMessage>(port_, GET_PORT(PNODE_ID_START));
  new_msg->SetData(*r);
  out_msg_.push(new_msg);
}

void Gate::HandleMessage_Active(std::shared_ptr<ActiveMessage> msg) {
  // Handle the first request belongs to tenent a.tenent
  ActiveData a;
  msg->GetData(a);
  auto r = requests_[a.tenent]->pop();
  boost::this_thread::sleep_for(boost::chrono::milliseconds(r->hardness));
  // Respond to PNode and User with a CompleteMessage
  std::shared_ptr<Message> new_msg_u = std::make_shared<CompleteMessage>(port_, GET_PORT(r->user));
  std::shared_ptr<Message> new_msg_p = std::make_shared<CompleteMessage>(port_, GET_PORT(PNODE_ID_START));
  CompleteData c = {
    .id = r->id,
    .tenent = r->tenent,
    .gate = node_id_,
    .status = 1,
  };
  new_msg_u->SetData(c);
  new_msg_p->SetData(c);
  out_msg_.push(new_msg_u);
  out_msg_.push(new_msg_p);
}

void Gate::Run() {
  thread_pool_.emplace_back( [=]{
    while(true) {
    }
  });
  Node::Run();
}

void User::HandleMessage_Complete(std::shared_ptr<CompleteMessage> msg) {
  // Print the total time of request execution
  CompleteData c;
  msg->GetData(c);
  long long create_time = req_send_time_.erase(c.id);
  long long complete_time = msg->GetCreateTime();
  std::cout << node_id_ << "\t"
            << c.id << "\t"
            << complete_time - create_time << "\t"
            << std::endl;
}

void User::Run() {
  thread_pool_.emplace_back( [=]{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rand_gate(GATE_ID_START, GATE_ID_END-1);
    std::normal_distribution<> rand_time{1000, 100};
    std::normal_distribution<> rand_hardness{100, 10};
    int msg_id = 0;

    while(true) {
      int gate_id = rand_gate(gen);
      int time_interval = rand_time(gen);
      int hardness_val = rand_hardness(gen);
      boost::this_thread::sleep_for(boost::chrono::milliseconds(time_interval));
      std::shared_ptr<Message> msg = std::make_shared<RequestMessage>(port_, GET_PORT(gate_id));
      RequestData r = {
        .id = msg_id,
        .user = node_id_,
        .tenent = tenent_id_,
        .gate = gate_id,
        .hardness = hardness_val,
      };
      msg->SetData(r);
      out_msg_.push(msg);
      req_send_time_.insert(msg_id, msg->GetCreateTime());
      msg_id ++;
    }
  });
  Node::Run();
}
