#ifndef PYSCHEDULE_NODE_H_
#define PYSCHEDULE_NODE_H_

#include <stdio.h>

#include "include/macros.h"
#include "include/lib_includes.h"
#include "include/message.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

class Node : public std::enable_shared_from_this<Node> {

  using shared_socket_t = std::shared_ptr<boost::asio::ip::tcp::socket>;

 public:
  explicit Node(int node_id, boost::asio::io_service& service);
  ~Node() {};

  void Run();

 private:

  bool IsValidID(int node_id);
  void SendMessage(std::shared_ptr<Message> msg);
  void RecvMessage(shared_socket_t socket,
    boost::system::error_code const& error);

  int node_id_;
  short port_;

  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;

  std::vector<std::thread> thread_pool_;
  std::mutex in_mutex_;
  std::mutex out_mutex_;
  std::queue<std::shared_ptr<Message> > in_msg_;
  std::queue<std::shared_ptr<Message> > out_msg_;

  DISALLOW_COPY_AND_ASSIGN(Node);
};


#endif // PYSCHEDULE_NODE_H_
