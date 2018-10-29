#include <string.h>
#include "include/node.h"

Node::Node(int node_id, boost::asio::io_service& service) 
  : io_service_(service), acceptor_(service) {
  node_id_ = node_id;
  port_ = PORT_BASE + node_id;
} 

void Node::SendMessage(std::shared_ptr<Message> msg) {
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::socket socket(io_service_);
  boost::asio::ip::tcp::resolver::iterator endpoint = 
    resolver.resolve(boost::asio::ip::tcp::resolver::query(HOST, std::to_string(msg->GetDstPort())));
  boost::asio::connect(socket, endpoint);
  socket.send(boost::asio::buffer(msg->ToString()));
}

void Node::ReadMessage(const boost::system::error_code& error) {
  if (!error) {
    auto msg = std::make_shared<Message>(socket_buffer);
    AtomicPushInMessage(msg);
  }
}

void Node::AtomicPushInMessage(std::shared_ptr<Message> msg) {
  std::lock_guard<std::mutex> guard(in_mutex_);
  in_msg_.push(msg);
}

std::shared_ptr<Message> Node::AtomicPopInMessage() {
  std::shared_ptr<Message> msg = nullptr;
  std::lock_guard<std::mutex> guard(in_mutex_);
  if (not in_msg_.empty()) {
    msg = in_msg_.front();
    in_msg_.pop();
  }
  return msg;
}

void Node::AtomicPushOutMessage(std::shared_ptr<Message> msg) {
  std::lock_guard<std::mutex> guard(out_mutex_);
  out_msg_.push(msg);
}

std::shared_ptr<Message> Node::AtomicPopOutMessage() {
  std::shared_ptr<Message> msg = nullptr;
  std::lock_guard<std::mutex> guard(out_mutex_);
  if (not out_msg_.empty()) {
    msg = out_msg_.front();
    out_msg_.pop();
  }
  return msg;
}

void Node::RecvMessage(shared_socket_t socket,
  boost::system::error_code const& error) {

  if (error) return;
  // Push the new messagoutinto in_msg_ queue
  socket->async_read_some(boost::asio::buffer(socket_buffer, MESSAGE_SIZE_MAX),
    boost::bind(&Node::ReadMessage, shared_from_this(), boost::asio::placeholders::error));
  shared_socket_t new_socket = std::make_shared<boost::asio::ip::tcp::socket>(io_service_);
  acceptor_.async_accept(*new_socket, [=](boost::system::error_code e) { RecvMessage(new_socket, e); });
}

void Node::Run() {
  auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_service_);
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port_);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  acceptor_.async_accept(*socket, [=](boost::system::error_code e) { RecvMessage(socket, e); });

  // Threads for receiving messages
  for (int i = 0; i < THREAD_NUM; ++i) {
    thread_pool_.emplace_back( [=]{ io_service_.run(); } );
  }

  // Threads for handling messages in the in_msg_ queue
  thread_pool_.emplace_back( [=]{
    //boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    std::shared_ptr<Message> msg = nullptr;
    while(true) {
      if ((msg = AtomicPopInMessage()) != nullptr) {
        HandleMessage(msg);
      }
    }
  });

  // Threads for sending messages in the out_msg_ queue
  thread_pool_.emplace_back( [=]{
    //boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    std::shared_ptr<Message> msg = nullptr;
    while(true) {
      if ((msg = AtomicPopOutMessage()) != nullptr) {
        SendMessage(msg);
      }
    }
  });

  for (std::thread& th: thread_pool_) {
    if (th.joinable())
      th.join();
  }
}

