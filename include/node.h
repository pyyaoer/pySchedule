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
     : io_service_(service), socket_(service), bytes_read(0) {}
    boost::asio::ip::tcp::socket& GetSocket() { return socket_; }
    void DoRead(std::shared_ptr<Node> node) {
      socket_.async_read_some(boost::asio::buffer(socket_buffer, MESSAGE_SIZE_MAX),
        boost::bind(&ConnectionHandler::HandleRead, shared_from_this(), node, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    void HandleRead(std::shared_ptr<Node> node, const boost::system::error_code& error,
                    std::size_t bytes_transferred);
  
   private:
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::socket socket_;
    char socket_buffer[MESSAGE_SIZE_MAX];
    std::size_t bytes_read;
  };
  using shared_handler_t = std::shared_ptr<ConnectionHandler>;

  class LatencyHandler : public std::enable_shared_from_this<LatencyHandler> {
   public:
    explicit LatencyHandler(boost::asio::io_service& service, int wait) :
        timer(service, boost::posix_time::milliseconds(wait)) {}

    void AddLatency(std::shared_ptr<Node> node, std::shared_ptr<Message> msg) {
      timer.async_wait(boost::bind(&LatencyHandler::HandleLatency, shared_from_this(),
                 node, msg, boost::asio::placeholders::error));
    }
    void HandleLatency(std::shared_ptr<Node> node, std::shared_ptr<Message> msg, const boost::system::error_code& error) {
      if (error) {
        std::cout << error.message() << std::endl;
      }
      node->HandleMessage(msg);
    }

   private:
    boost::asio::deadline_timer timer;
  };

 public:
  explicit Node(int node_id, boost::asio::io_service& service): Node(node_id, 0, service) {}
  explicit Node(int node_id, int msg_latency, boost::asio::io_service& service)
   : io_service_(service), msg_latency_(msg_latency), acceptor_(service),
     node_id_(node_id), port_(GET_PORT(node_id)) {}
  ~Node() {};

  void Run();

 protected:
  int node_id_;
  int msg_latency_;
  short port_;
  boost::asio::io_service& io_service_;
  std::vector<std::thread> thread_pool_;
  SafeQueue<std::shared_ptr<Message> > in_msg_;
  SafeQueue<std::shared_ptr<Message> > out_msg_;

  inline void SendMessage(std::shared_ptr<Message> msg) {
    out_msg_.push(msg);
  };

 private:
  // Recv -> Read -> Handle -> Send
  void RecvMessage(shared_handler_t handler,
    boost::system::error_code const& error);
  virtual void HandleMessage(std::shared_ptr<Message> msg) = 0;
  void SendMessageInternal(std::shared_ptr<Message> msg);

  boost::asio::ip::tcp::acceptor acceptor_;

  DISALLOW_COPY_AND_ASSIGN(Node);
};

#endif // PYSCHEDULE_NODE_H_
