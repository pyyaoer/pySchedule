#include <random>
#include "nodes.h"
#include "include/ddlsession.h"

#define CLOCK_REMAINDER 10000000

BOOST_CLASS_EXPORT(RequestMessage)
BOOST_CLASS_EXPORT(TagMessage)
BOOST_CLASS_EXPORT(ScheduledMessage)
BOOST_CLASS_EXPORT(CompleteMessage)

void PNode::HandleMessage_Request(std::shared_ptr<RequestMessage> msg) {
  //std::cout << "PNode::HandleMessage_Request" << std::endl;
  RequestData r;
  msg->GetData(r);
  std::shared_ptr<Message> new_msg = std::make_shared<TagMessage>(port_, GET_PORT(GID2NID(r.gate)));
  TagData t = {
    .rho = rho_[r.gate][r.tenant],
    .delta = delta_[r.gate][r.tenant],
    .request_gid = r.request_gid,
  };
  new_msg->SetData(t);
  out_msg_.push(new_msg);
}

void PNode::HandleMessage_Scheduled(std::shared_ptr<ScheduledMessage> msg) {
  //std::cout << "PNode::HandleMessage_Scheduled" << std::endl;
  ScheduledData s;
  msg->GetData(s);
  // Update the rho records if the request was treated as a constraint-based one
  if (s.method == 0) {
    // Reset the record of the current gate
    rho_[s.gate][s.tenant] = 1;
    // Update the records of the other gates
    for (int i = 0; i < GATE_NUM; ++i) {
      if (i == s.gate) continue;
      rho_[i][s.tenant] ++;
    }
  }
  // Update the delta records
  delta_[s.gate][s.tenant] = 1;
  for (int i = 0; i < GATE_NUM; ++i) {
    if (i == s.gate) continue;
    delta_[i][s.tenant] ++;
  }
}

void PNode::Run() {
  Node::Run();
}

void Gate::HandleMessage_Request(std::shared_ptr<RequestMessage> msg) {
  //std::cout << "Gate::HandleMessage_Request" << std::endl;
  // Push the new request from User(tenant t) to the requests_[t] queue
  auto r = std::make_shared<RequestData>();
  msg->GetData(*r);
  r->request_gid = request_id_++;
  requests_->push(r);
  // Send a RequestMessage to PNode
  std::shared_ptr<Message> new_msg = std::make_shared<RequestMessage>(port_, GET_PORT(PNODE_ID_START));
  new_msg->SetData(*r);
  out_msg_.push(new_msg);
}
 
void Gate::HandleMessage_Tag(std::shared_ptr<TagMessage> msg) {
  //std::cout << "Gate::HandleMessage_Tag" << std::endl;
  std::unique_lock lock(tag_mutex_);
  TagData t;
  msg->GetData(t);
  RequestData r;
  std::function<bool(RequestData)> lambda_match =
    [t](RequestData rd)->bool {
      return t.request_gid == rd.request_gid;
    };
  requests_->erase_match(r, lambda_match);
  long long now = (duration_cast< milliseconds >(system_clock::now().time_since_epoch())).count();
  double dnow = now % CLOCK_REMAINDER;
  rtag_[r.tenant] = std::max(rtag_[r.tenant] + t.rho / TENANT_RESERVATION, dnow);
  ltag_[r.tenant] = std::max(ltag_[r.tenant] + t.delta / TENANT_LIMIT, dnow);
  if (ptag_[r.tenant] == 0) {
    long long mn = LLONG_MAX;
    for(auto p : ptag_) {
      if (p != 0 and mn > p) {
        mn = p;
      }
    }
    for(auto &p : ptag_) {
      if (p != 0) {
        p -= mn;
      }
    }
  }
  ptag_[r.tenant] = std::max(ptag_[r.tenant] + 1, dnow);
  auto item = std::make_shared<TodoItem>(
    TodoItem {
      .r_tag = rtag_[r.tenant],
      .l_tag = ltag_[r.tenant],
      .p_tag = ptag_[r.tenant],
      .data = r,
    }
  );
  todo_list_->push(item);
}

void Gate::Run() {
  thread_pool_.emplace_back( [=]{
    // Try to do the scheduling
    while (true) {
      // Wait for an idle slot
      idle_slots_->pop();
      // Search for the minimum r_tag
      TodoItem t;
      int min_rid = -1;
      long long min_rtag = LLONG_MAX;
      int min_pid = -1;
      long long min_ptag = LLONG_MAX;
      long long now = (duration_cast< milliseconds >(system_clock::now().time_since_epoch())).count();
      std::function<bool(TodoItem)> lambda_scan =
        [&, now](TodoItem ti)->bool {
          if (min_rtag > ti.r_tag) {
            min_rtag = ti.r_tag;
            min_rid = ti.data.request_gid;
          }
          if (ti.l_tag <= now and min_ptag > ti.p_tag) {
            min_ptag = ti.p_tag;
            min_pid = ti.data.request_gid;
          }
          return false;
        };
      todo_list_->erase_match(t, lambda_scan);

      int schedule_method = -1;
      if(min_rtag <= now) {
        // constraint-based scheduling
        schedule_method = 0;
        std::function<bool(TodoItem)> lambda_r =
          [min_rid](TodoItem ti)->bool {
            return ti.data.request_gid == min_rid;
          };
        todo_list_->erase_match(t, lambda_r);
      }
      else if (min_ptag < LLONG_MAX) {
        // weight-based scheduling
        schedule_method = 1;
        std::function<bool(TodoItem)> lambda_p =
          [min_pid](TodoItem ti)->bool {
            return ti.data.request_gid == min_pid;
          };
        todo_list_->erase_match(t, lambda_p);
	// Subtract 1/r from R tags of the same tenant
	std::function<bool(TodoItem&)> lambda_update =
          [t](TodoItem& ti)->bool {
	    if (ti.data.tenant == t.data.tenant) {
	      ti.r_tag -= 1 / TENANT_RESERVATION;
	    }
            return false;
          };
      todo_list_->erase_match(t, lambda_scan);
      }

      if (schedule_method < 0) {
        idle_slots_->push(true);
        continue;
      }

      // Do the IO job
      auto self(shared_from_this());
      RequestData rd = t.data;
      std::shared_ptr<Message> new_msg_u = std::make_shared<CompleteMessage>(port_, GET_PORT(rd.user));
      std::shared_ptr<Message> new_msg_p = std::make_shared<ScheduledMessage>(port_, GET_PORT(PNODE_ID_START));
      CompleteData c = {
        .id = rd.id,
        .tenant = rd.tenant,
        .gate = NID2GID(node_id_),
        .status = 1,
      };
      new_msg_u->SetData(c);
      ScheduledData s = {
        .tenant = rd.tenant,
        .gate = NID2GID(node_id_),
        .method = schedule_method,
      };
      new_msg_p->SetData(s);
      auto handler = [&, new_msg_u, new_msg_p, self]() {
        // Respond to User with a CompleteMessage
        out_msg_.push(new_msg_u);
        out_msg_.push(new_msg_p);
        idle_slots_->push(true);
      };
      std::make_shared<DDLSession>(io_service_, handler, rd.hardness)->start();
    }
  });
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
    std::normal_distribution<> rand_time{100, 10};
    std::normal_distribution<> rand_hardness{100, 10};

    while(true) {
      int gate_node_id = rand_gate(gen);
      int time_interval = rand_time(gen);
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
      out_msg_.push(msg);
      req_send_time_.insert(msg_id_, msg->GetCreateTime());
      msg_id_ ++;
    }
  });
  Node::Run();
}

