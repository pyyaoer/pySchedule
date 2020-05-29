#include "nodes.h"
#include "include/ddlsession.h"

BOOST_CLASS_EXPORT(TagsMessage)
BOOST_CLASS_EXPORT(SyncMessage)

long long get_now() {
  return (duration_cast< milliseconds >(system_clock::now().time_since_epoch())).count() - TIME_START;
}

void PNode::HandleMessage_Sync(std::shared_ptr<SyncMessage> msg) {
  //std::cout << "PNode::HandleMessage_Sync" << std::endl;
  SyncData s;
  msg->GetData(s);
  TagsData t;
  memset(t.rho, -1, sizeof(t.rho));
  memset(t.delta, -1, sizeof(t.delta));
  long long now = get_now();
  for (int i = 0; i < TENANT_NUM; ++i) {
    if (s.rnum[i] >= 0) {
      int n = record_r_[i].UpdateAndCount(s.gate, s.rnum[i], now-s.period, now);
      t.rho[i] = (s.rnum[i] == 0) ? 0 : ((double)n) / s.rnum[i];
    }
    if (s.dnum[i] >= 0) {
      int n = record_d_[i].UpdateAndCount(s.gate, s.dnum[i], now-s.period, now);
      t.delta[i] = (s.dnum[i] == 0) ? 0 : ((double)n) / s.dnum[i];
    }
  }
  std::shared_ptr<Message> new_msg = std::make_shared<TagsMessage>(port_, GET_PORT(GID2NID(s.gate)));
  new_msg->SetData(t);
  SendMessage(new_msg);
}

void PNode::Run() {
  Node::Run();
}

void Gate::HandleMessage_Request(std::shared_ptr<RequestMessage> msg) {
  //std::cout << "Gate::HandleMessage_Request" << std::endl;
  RequestData r;
  msg->GetData(r);
  request_id_++;
  double dnow = get_now();
  // use parameters automatically
  {
    std::unique_lock lock(parameter_mutex_);
    rtag_[r.tenant] = std::max(rtag_[r.tenant] + 1000.0 * rho_[r.tenant] / TENANT_RESERVATION, dnow);
    ltag_[r.tenant] = std::max(ltag_[r.tenant] + 1000.0 * delta_[r.tenant] / TENANT_LIMIT, dnow);
  }
  if (ptag_[r.tenant] < 0.01) {
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
  if (DBG >= 1) {
    std::cout << "\tGate:" << r.gate
              << "\tTenant:" << r.tenant
              << "\tID:" <<  r.id
	      << "\tRTag:" << rtag_[r.tenant]
	      << "\tLTag:" << ltag_[r.tenant]
	      << "\tPTag:" << ptag_[r.tenant]
	      << std::endl;
  }
  todo_list_->push(item);
}
 
void Gate::HandleMessage_Tags(std::shared_ptr<TagsMessage> msg) {
  //std::cout << "Gate::HandleMessage_Tags" << std::endl;
  TagsData t;
  msg->GetData(t);
  // update parameters
  {
    std::unique_lock lock(parameter_mutex_);
    for (int i = 0; i < TENANT_NUM; ++i) {
      rho_[i] = t.rho[i];
      delta_[i] = t.delta[i];
    }
  }
}

void Gate::Run() {
  thread_pool_.emplace_back( [=]{
    // Send sync messages periodically to PNode
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(period_));
      std::shared_ptr<Message> new_msg = std::make_shared<SyncMessage>(port_, GET_PORT(PNODE_ID_START));
      SyncData s;
      s.gate = NID2GID(node_id_);
      s.period = period_;
      for (int i = 0; i < TENANT_NUM; ++i) {
        s.rnum[i] = scheduled_[i][0];
        s.dnum[i] = scheduled_[i][1];
        scheduled_[i][0] = 0;
        scheduled_[i][1] = 0;
      }
      new_msg->SetData(s);
      SendMessage(new_msg);
    }
  });
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
      long long now = get_now();
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
              ti.r_tag -= (double) 1 / TENANT_RESERVATION;
            }
            return false;
          };
        todo_list_->erase_match(t, lambda_scan);
      }

      // Do the IO job
      auto self(shared_from_this());
      RequestData rd = t.data;
      if (schedule_method < 0) {
        idle_slots_->push(true);
        continue;
      }
      scheduled_[rd.tenant][schedule_method]++;

      std::shared_ptr<Message> new_msg = std::make_shared<CompleteMessage>(port_, GET_PORT(rd.user));
      CompleteData c = {
        .id = rd.id,
        .tenant = rd.tenant,
        .gate = NID2GID(node_id_),
        .status = 1,
      };
      new_msg->SetData(c);
      auto handler = [&, new_msg, self]() {
        // Respond to User with a CompleteMessage
        SendMessage(new_msg);
        idle_slots_->push(true);
      };
      std::make_shared<DDLSession>(io_service_, handler, rd.hardness)->start();
    }
  });
  Node::Run();
}

