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
  //std::cout << "Send from " << port_ << " to " << msg->GetDstPort() << std::endl;
  //socket.close();
}

void Node::RecvMessage(shared_handler_t handler,
  boost::system::error_code const& error) {

  if (!error)
  // Push the new messagoutinto in_msg_ queue
    handler->DoRead(shared_from_this());
  shared_handler_t new_handler = std::make_shared<ConnectionHandler>(io_service_);
  acceptor_.async_accept(new_handler->GetSocket(), [this, new_handler](const boost::system::error_code &e) { RecvMessage(new_handler, e); });
}

void Node::Run() {
  auto handler = std::make_shared<ConnectionHandler>(io_service_);
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port_);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen(boost::asio::socket_base::max_connections);
  acceptor_.async_accept(handler->GetSocket(), [this, handler](const boost::system::error_code &e) { RecvMessage(handler, e); });

  // Threads for receiving messages
  for (int i = 0; i < THREAD_NUM; ++i) {
    thread_pool_.emplace_back( [this]{ io_service_.run(); } );
  }

  // Threads for handling messages in the in_msg_ queue
  thread_pool_.emplace_back( [this]{
    // Waiting for all nodes to be alive
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    while(true) {
      HandleMessage(in_msg_.pop());
    }
  });

  // Threads for sending messages in the out_msg_ queue
  thread_pool_.emplace_back( [this]{
    // Waiting for all nodes to be alive
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    while(true) {
      SendMessage(out_msg_.pop());
    }
  });

  for (std::thread& th: thread_pool_) {
    if (th.joinable())
      th.join();
  }
}

void Node::ConnectionHandler::HandleRead(std::shared_ptr<Node> node, const boost::system::error_code& error,
                                         std::size_t bytes_transferred) {
  if (!error)
  {
    bytes_read += bytes_transferred;
    socket_.async_read_some(boost::asio::buffer(socket_buffer, MESSAGE_SIZE_MAX) + bytes_read,
        boost::bind(&ConnectionHandler::HandleRead, shared_from_this(), node, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    return;
  }
  try {
    std::shared_ptr<Message> msg = nullptr;
    std::string archive_data(socket_buffer, MESSAGE_SIZE_MAX);
    std::istringstream archive_stream(archive_data);
    boost::archive::text_iarchive archive(archive_stream);
    archive >> msg;
    node->in_msg_.push(msg);
  }
  catch (const std::exception& e) {
    std::cout << "Receive Message Error: " << e.what() << std::endl;
    return;
  }
}
