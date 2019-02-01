#ifndef PYSCHEDULE_NODE_H_
#define PYSCHEDULE_NODE_H_

#include <stdio.h>

#include "include/macro.h"
#include "include/lib_include.h"
#include "include/message.h"
#include "include/safequeue.h"
#include "include/safemap.h"
#include "include/safelist.h"

class Node : public std::enable_shared_from_this<Node> {

  class ConnectionHandler : public std::enable_shared_from_this<ConnectionHandler> {
   public:
    explicit ConnectionHandler(boost::asio::io_service& service)
     : io_service_(service), socket_(service) {}
    boost::asio::ip::tcp::socket& GetSocket() { return socket_; }
    void DoRead(std::shared_ptr<Node> node) {
      socket_.async_read_some(boost::asio::buffer(socket_buffer, MESSAGE_SIZE_MAX),
        boost::bind(&ConnectionHandler::HandleRead, shared_from_this(), node, boost::asio::placeholders::error));
    }
    void HandleRead(std::shared_ptr<Node> node, const boost::system::error_code& error);
  
   private:
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::socket socket_;
    char socket_buffer[MESSAGE_SIZE_MAX];
  };
  using shared_handler_t = std::shared_ptr<ConnectionHandler>;

  struct SeenMsg {
    SeenMsg(): SeenMsg(-1, -1) {}
    SeenMsg(int mid, short pid): msg_id(mid), port_id(pid) {}
    int msg_id;
    short port_id;
  };
  using SeenMsgList = SafeList<std::shared_ptr<SeenMsg> >;


 public:
  explicit Node(int node_id, boost::asio::io_service& service)
   : io_service_(service), acceptor_(service), message_id_(0),
     node_id_(node_id), port_(GET_PORT(node_id)) {
       seen_msg_ = std::make_shared<SeenMsgList>();
     }
  ~Node() {};

  void Run();

 protected:
  int node_id_;
  short port_;
  int message_id_;
  std::shared_ptr<SeenMsgList> seen_msg_;
  boost::asio::io_service& io_service_;
  std::vector<std::thread> thread_pool_;
  SafeQueue<std::shared_ptr<Message> > in_msg_;
  SafeQueue<std::shared_ptr<Message> > out_msg_;

 private:
  // Recv -> Read -> Handle -> Send
  void RecvMessage(shared_handler_t handler,
    boost::system::error_code const& error);
  virtual void HandleMessage(std::shared_ptr<Message> msg) = 0;
  void SendMessage(std::shared_ptr<Message> msg);

  boost::asio::ip::tcp::acceptor acceptor_;

  DISALLOW_COPY_AND_ASSIGN(Node);
};

#endif // PYSCHEDULE_NODE_H_
