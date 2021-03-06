#include <random>
#include "nodes.h"
#include "include/ddlsession.h"

BOOST_CLASS_EXPORT(RequestMessage)
BOOST_CLASS_EXPORT(CompleteMessage)
BOOST_CLASS_EXPORT(ActiveMessage)

void PNode::HandleMessage_Request(std::shared_ptr<RequestMessage> msg) {
  //std::cout << "PNode::HandleMessage_Request" << std::endl;
  // Push the new request to the todo list
  RequestData r;
  msg->GetData(r);
  auto item = std::make_shared<TodoItem>(
    TodoItem {
      .gate = r.gate,
      .tenant = r.tenant,
      .time = msg->GetCreateTime(),
    }
  );
  todo_list_->push(item);
}

void PNode::HandleMessage_Complete(std::shared_ptr<CompleteMessage> msg) {
  //std::cout << "PNode::HandleMessage_Complete" << std::endl;
  CompleteData c;
  msg->GetData(c);
  idle_list_[c.gate]->push(true);
}

void PNode::Run() {
  thread_pool_.emplace_back( [=]{
    int d[TENANT_NUM] = {0};
    // Try to do the scheduling
    while (true) {
      // Tenants that are under reservation
      bool r_schedule = false;
      bool l_schedule = false;
      int r[TENANT_NUM] = {0};
      int l[TENANT_NUM] = {0};
      long long now = (duration_cast< milliseconds >(system_clock::now().time_since_epoch())).count();
      long long sleep_time = INT64_MAX;
      for (int i = 0; i < TENANT_NUM; i++) {
        // Delete jobs from done_list_ which is out of the current window
        auto dl = done_list_[i];
        while(!dl->empty() and (now - dl->back() > time_window_)) {
          dl->pop();
        }
        if (!dl->empty())
          sleep_time = std::min(sleep_time, time_window_ - (now - dl->back()));
        // Count the number of done requests in the current window for each tenant
        // and compare it with reservation demand
        d[i] = dl->size();
        r[i] = TENANT_RESERVATION * time_window_ / 1000 - d[i];
        l[i] = TENANT_LIMIT * time_window_ / 1000 - d[i];
        r_schedule = r_schedule or (r[i] > 0);
        l_schedule = l_schedule or (l[i] > 0);
      }
      // Find idle gates
      int g[GATE_NUM] = {0};
      bool g_schedule;
      for (int i = 0; i < GATE_NUM; i++) {
        g[i] = idle_list_[i]->size();
        g_schedule = g_schedule or (g[i] > 0);
      }
      if (l_schedule and g_schedule) {
        TodoItem t;
       // Some tenants haven't met their reservation during last window
       // Make them happy first
        std::function<bool(TodoItem)> lambda_r =
          [&r, &g](TodoItem ti)->bool {
            return r[ti.tenant] > 0 and g[ti.gate] > 0;
          };
        while (todo_list_->erase_match(t, lambda_r)) {
          r[t.tenant] --;
          l[t.tenant] --;
          g[t.gate] --;
          idle_list_[t.gate]->pop();
          done_list_[t.tenant]->push(now);
          ActiveData a = {
            .tenant = t.tenant,
          };
          std::shared_ptr<Message> new_msg = std::make_shared<ActiveMessage>(port_, GET_PORT(GID2NID(t.gate)));
          new_msg->SetData(a);
	  SendMessage(new_msg);
        }
        // After trying to meet the reservation demand,
        // schedule some requests before reaching limit
        std::function<bool(TodoItem)> lambda_l =
          [&l, &g](TodoItem ti)->bool {
            return l[ti.tenant] > 0 and g[ti.gate] > 0;
          };
        while (todo_list_->erase_match(t, lambda_l)) {
          l[t.tenant] --;
          g[t.gate] --;
          idle_list_[t.gate]->pop();
          done_list_[t.tenant]->push(now);
          ActiveData a = {
            .tenant = t.tenant,
          };
          std::shared_ptr<Message> new_msg = std::make_shared<ActiveMessage>(port_, GET_PORT(GID2NID(t.gate)));
          new_msg->SetData(a);
	  SendMessage(new_msg);
        }
      }
      // Sleep
      {
        if (sleep_time == INT64_MAX) // nothing to do
          sleep_time = 1000;
        std::unique_lock<std::mutex> lck(cv_mtx);
        if (!new_incoming_msg_flag)
          cv.wait_for(lck, std::chrono::milliseconds(sleep_time));
        new_incoming_msg_flag = false;
      }
    }
  });
  Node::Run();
}


void Gate::HandleMessage_Request(std::shared_ptr<RequestMessage> msg) {
  //std::cout << "Gate::HandleMessage_Request" << std::endl;
  // Push the new request from User(tenant t) to the requests_[t] queue
  auto r = std::make_shared<RequestData>();
  msg->GetData(*r);
  requests_[r->tenant]->push(r);
  // Send a RequestMessage to PNode
  std::shared_ptr<Message> new_msg = std::make_shared<RequestMessage>(port_, GET_PORT(PNODE_ID_START));
  new_msg->SetData(*r);
  SendMessage(new_msg);
}
 
void Gate::HandleMessage_Active(std::shared_ptr<ActiveMessage> msg) {
  //std::cout << "Gate::HandleMessage_Active" << std::endl;
  // Handle the first request belongs to tenant a.tenant
  ActiveData a;
  msg->GetData(a);
  auto r = requests_[a.tenant]->pop();

  auto self(shared_from_this());
  auto handler = [&, r, self]() {
    // Respond to PNode and User with a CompleteMessage
    std::shared_ptr<Message> new_msg_u = std::make_shared<CompleteMessage>(port_, GET_PORT(r->user));
    std::shared_ptr<Message> new_msg_p = std::make_shared<CompleteMessage>(port_, GET_PORT(PNODE_ID_START));
    CompleteData c = {
      .id = r->id,
      .tenant = r->tenant,
      .gate = NID2GID(node_id_),
      .status = 1,
    };
    new_msg_u->SetData(c);
    new_msg_p->SetData(c);
    SendMessage(new_msg_u);
    SendMessage(new_msg_p);
  };
  std::make_shared<DDLSession>(io_service_, handler, r->hardness)->start();
}

void Gate::Run() {
  Node::Run();
}

void User::HandleMessage_Complete(std::shared_ptr<CompleteMessage> msg) {
  //std::cout << "User::HandleMessage_Complete" << std::endl;
  // Print the total time of request execution
  CompleteData c;
  msg->GetData(c);
  long long create_time = req_send_time_.erase(c.id);
  long long complete_time = (duration_cast< milliseconds >(system_clock::now().time_since_epoch())).count();
  long long latency = complete_time - create_time;
  latency_summary_ += latency;
  counter_ ++;
  if (maximum_latency_ < latency)
    maximum_latency_ = latency;
  std::cout << node_id_ << "\t"
            << c.id << "\t"
            << latency << "\t"
            << latency_summary_ / counter_ << "\t"
            << maximum_latency_ << "\t"
            << std::endl;
}

void User::Run() {
  thread_pool_.emplace_back( [=]{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rand_gate(GATE_ID_START, GATE_ID_END - 1);
    std::normal_distribution<> rand_hardness{100, 10};

    while(true) {
      int gate_node_id = rand_gate(gen);
      int time_interval = bcmk_.next();
      int hardness_val = rand_hardness(gen);
      std::this_thread::sleep_for(std::chrono::milliseconds(time_interval));
      std::shared_ptr<Message> msg = std::make_shared<RequestMessage>(port_, GET_PORT(gate_node_id));
      RequestData r = {
        .id = msg_id_,
        .user = node_id_,
        .tenant = tenant_id_,
        .gate = NID2GID(gate_node_id),
        .hardness = hardness_val,
      };
      msg->SetData(r);
      SendMessage(msg);
      req_send_time_.insert(msg_id_, msg->GetCreateTime());
      msg_id_ ++;
    }
  });
  Node::Run();
}
