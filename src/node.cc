#include <string.h>
#include "include/node.h"

void Node::SendMessage(std::shared_ptr<Message> msg) {
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::socket socket(io_service_);

  msg->SetID(message_id_);
  for (int i = 0; i < 2; ++i) {
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
    socket.close();
  }
  message_id_ ++;
}

void Node::RecvMessage(shared_handler_t handler,
  boost::system::error_code const& error) {

  if (error) return;
  // Push the new messagoutinto in_msg_ queue
  handler->DoRead(shared_from_this());
  shared_handler_t new_handler = std::make_shared<ConnectionHandler>(io_service_);
  acceptor_.async_accept(new_handler->GetSocket(), [=](boost::system::error_code e) { RecvMessage(new_handler, e); });
}

void Node::Run() {
  auto handler = std::make_shared<ConnectionHandler>(io_service_);
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port_);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen(boost::asio::socket_base::max_connections);
  acceptor_.async_accept(handler->GetSocket(), [=](boost::system::error_code e) { RecvMessage(handler, e); });

  // Threads for receiving messages
  for (int i = 0; i < THREAD_NUM; ++i) {
    thread_pool_.emplace_back( [=]{ io_service_.run(); } );
  }

  // Threads for handling messages in the in_msg_ queue
  thread_pool_.emplace_back( [=]{
    // Waiting for all nodes to be alive
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    while(true) {
      HandleMessage(in_msg_.pop());
    }
  });

  // Threads for sending messages in the out_msg_ queue
  thread_pool_.emplace_back( [=]{
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

void Node::ConnectionHandler::HandleRead(std::shared_ptr<Node> node, const boost::system::error_code& error) { 
  std::shared_ptr<Message> msg = nullptr;
  try {
    std::string archive_data(socket_buffer, MESSAGE_SIZE_MAX);
    std::istringstream archive_stream(archive_data);
    boost::archive::text_iarchive archive(archive_stream);
    archive >> msg;

    SeenMsg ret;
    auto sm = std::make_shared<SeenMsg>(msg->GetID(), msg->GetSrcPort());
    std::function<bool(SeenMsg)> lambda_msg =
      [sm](SeenMsg m)->bool {
        return m.msg_id == sm->msg_id and
               m.port_id == sm->port_id;
      };
    if (!node->seen_msg_->erase_match(ret, lambda_msg)) {
      node->seen_msg_->push(sm);
      node->in_msg_.push(msg);
    }
  }
  catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    return;
    for (int i = 0; i < MESSAGE_SIZE_MAX; ++i) {
      std::cout << (int)socket_buffer[i];
    }
    std::cout << std::endl;
    if (msg == nullptr) {
      std::cout << "????" << std::endl;
    }
  }
}
