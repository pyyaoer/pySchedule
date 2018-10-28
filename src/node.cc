#include "include/node.h"

Node::Node(int node_id, boost::asio::io_service& service) 
  : io_service_(service), acceptor_(service) {
  if (IsValidID(node_id)) {
    node_id_ = -1;
    printf("Error! Node id %d is invalid", node_id);
    return;
  }
  node_id_ = node_id_;
  port_ = PORT_BASE + node_id;
} 

bool Node::IsValidID(int node_id) {
  return true;
}

void Node::SendMessage(std::shared_ptr<Message> msg) {
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::socket socket(io_service_);
  boost::asio::ip::tcp::resolver::iterator endpoint = 
    resolver.resolve(boost::asio::ip::tcp::resolver::query(HOST, msg->GetPort()));
  boost::asio::connect(socket, endpoint);
  socket.send(boost::asio::buffer(msg->ToString()));
}

void Node::RecvMessage(shared_socket_t socket,
  boost::system::error_code const& error) {

  if (error) return;

  // DO SOMETHING HERE!
  char data[MESSAGE_SIZE_MAX] = {0};
  auto self(shared_from_this());
  socket->async_read_some(boost::asio::buffer(data, MESSAGE_SIZE_MAX),
    [this, self, data](boost::system::error_code ec, std::size_t length)
    {
      if (!ec) {
        std::cout << data << std::endl;
      }
    });

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

  for (int i = 0; i < THREAD_NUM; ++i) {
    thread_pool_.emplace_back( [=]{ io_service_.run(); } );
  }

  thread_pool_.emplace_back( [=]{
    std::shared_ptr<Message> msg;
    while(true) {
      msg = nullptr;
      {
        std::lock_guard<std::mutex> guard(in_mutex_);
        if (not in_msg_.empty()) {
          msg = in_msg_.front();
          in_msg_.pop();
        }
      }
      if (msg != nullptr) {
        //DO SOMETHING WITH msg!
        std::cout << msg->ToString() << std::endl;
      }
    }
  });

  thread_pool_.emplace_back( [=]{
    std::shared_ptr<Message> msg;
    while(true) {
      msg = nullptr;
      {
        std::lock_guard<std::mutex> guard(out_mutex_);
        if (not out_msg_.empty()) {
          msg = out_msg_.front();
          out_msg_.pop();
        }
      }
      if (msg != nullptr) {
        SendMessage(msg);
      }
    }
  });

  for (std::thread& th: thread_pool_) {
    if (th.joinable())
      th.join();
  }
}

