#include "unodes.h"

long long total_latency = 0;
int total_obey = 0;
int total_count = 0;

extern std::vector<std::queue<HistoryItem>> history;
std::mutex history_mutex;

BOOST_CLASS_EXPORT(RequestMessage)
BOOST_CLASS_EXPORT(CompleteMessage)

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
  //std::cout << node_id_ << "\t"
  //          << c.id << "\t"
  //          << latency << "\t"
  //          << latency_summary_ / counter_ << "\t"
  //          << maximum_latency_ << "\t"
  //          << std::endl;
  total_latency += latency;
  total_count ++;
  HistoryItem cur = {
    .create_time = create_time,
    .finish_time = complete_time,
  };
  {
    std::unique_lock lock(history_mutex);
    history[tenant_id_].push(cur);
    auto his = history[tenant_id_].front();
    history[tenant_id_].pop();
    if (cur.create_time - his.create_time >= 1000 * WINDOW_SIZE / TENANT_RESERVATION
	or cur.finish_time - his.finish_time <= 1010 * WINDOW_SIZE / TENANT_RESERVATION
	and cur.finish_time - his.finish_time >= 990 * WINDOW_SIZE / TENANT_LIMIT) {
      total_obey ++;
    }
    std::cout << total_latency / total_count << "\t"
            << total_obey * 100.0 / total_count << "%\t"
            << ((cur.create_time - his.create_time >= 1000 * WINDOW_SIZE / TENANT_RESERVATION) ? 1 : 0) << "\t"
            << ((cur.create_time - his.create_time <= 1010 * WINDOW_SIZE / TENANT_RESERVATION) or (cur.finish_time - his.finish_time <= 1000 * WINDOW_SIZE / TENANT_RESERVATION) ? 1 : 0) << "\t"
            << ((cur.create_time - his.create_time >= 990 * WINDOW_SIZE / TENANT_RESERVATION) or (cur.finish_time - his.finish_time >= 1000 * WINDOW_SIZE / TENANT_LIMIT) ? 1 : 0) << "\t"
            << std::endl;
  }
}

void User::Run() {
  thread_pool_.emplace_back( [=]{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rand_gate(GATE_ID_START, GATE_ID_END - 1);
    std::normal_distribution<> rand_hardness{(double)REQUEST_HARDNESS, (double)REQUEST_HARDNESS/10};

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

