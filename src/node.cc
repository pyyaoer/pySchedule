#include <string.h>
#include "include/node.h"

void Node::SendMessage(std::shared_ptr<Message> msg) {
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::socket socket(io_service_);
  boost::asio::ip::tcp::resolver::iterator endpoint = 
    resolver.resolve(boost::asio::ip::tcp::resolver::query(LOCALHOST, std::to_string(msg->GetDstPort())));
  boost::asio::connect(socket, endpoint);

  {
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << msg;
    socket.send(boost::asio::buffer(archive_stream.str()));
  }
  std::cout << "Sent!" << std::endl;
}

void Node::ReadMessage(const boost::system::error_code& error) {
  // TODO: Do not ignore the error here (but I observe that the error is always "Operation canceled" error which may be ignored)
  if (error) {
    //std::cout << "Something strange happened : " << error.message() << std::endl;
  }
  std::shared_ptr<Message> msg = nullptr;
  {
    std::string archive_data(socket_buffer, MESSAGE_SIZE_MAX);
    std::istringstream archive_stream(archive_data);
    boost::archive::text_iarchive archive(archive_stream);
    archive >> msg;
  }

  AtomicPushInMessage(msg);
  for (int i = 0; i < MESSAGE_SIZE_MAX; ++i) {
    std::cout << (int)socket_buffer[i];
  }
  std::cout << std::endl;
}

void AtomicPushMessage(std::shared_ptr<Message> msg, std::mutex &mutex, std::queue<std::shared_ptr<Message> > &queue) {
  std::lock_guard<std::mutex> guard(mutex);
  queue.push(msg);
}

std::shared_ptr<Message> AtomicPopMessage(std::mutex &mutex, std::queue<std::shared_ptr<Message> > &queue) {
  std::shared_ptr<Message> msg = nullptr;
  std::lock_guard<std::mutex> guard(mutex);
  if (not queue.empty()) {
    msg = queue.front();
    queue.pop();
  }
  return msg;
}

void Node::AtomicPushInMessage(std::shared_ptr<Message> msg) {
  AtomicPushMessage(msg, in_mutex_, in_msg_);
}

std::shared_ptr<Message> Node::AtomicPopInMessage() {
  return AtomicPopMessage(in_mutex_, in_msg_);
}

void Node::AtomicPushOutMessage(std::shared_ptr<Message> msg) {
  AtomicPushMessage(msg, out_mutex_, out_msg_);
}

std::shared_ptr<Message> Node::AtomicPopOutMessage() {
  return AtomicPopMessage(out_mutex_, out_msg_);
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
    std::shared_ptr<Message> msg = nullptr;
    while(true) {
      boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
      if ((msg = AtomicPopInMessage()) != nullptr) {
        HandleMessage(msg);
      }
    }
  });

  // Threads for sending messages in the out_msg_ queue
  thread_pool_.emplace_back( [=]{
    std::shared_ptr<Message> msg = nullptr;
    while(true) {
      boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
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

