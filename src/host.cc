#include "include/host.h"

Host::Host(int host_id, boost::asio::io_service& service) 
  : io_service_(service), acceptor_(service) {
  if (host_id >= HOST_NUM || host_id < 0) {
    host_id_ = -1;
    printf("Error! Host id %d is invalid", host_id);
    return;
  }
  host_id_ = host_id_;
  port_ = HOST_SEND_BASE + host_id;
} 


void Host::SendMessage(Message& msg) {
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::socket socket(io_service_);
  boost::asio::ip::tcp::resolver::iterator endpoint = 
    resolver.resolve(boost::asio::ip::tcp::resolver::query(HOST, msg.GetPort()));
  boost::asio::connect(socket, endpoint);
  socket.send(boost::asio::buffer(msg.ToString()));
}

void Host::RecvMessage(shared_socket_t socket,
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

void Host::Run() {
  auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_service_);
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port_);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  acceptor_.async_accept(*socket, [=](boost::system::error_code e) { RecvMessage(socket, e); });

  for (int i = 0; i < USER_NUM / HOST_NUM; ++i) {
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

  for (std::thread& th: thread_pool_) {
    if (th.joinable())
      th.join();
  }
}

