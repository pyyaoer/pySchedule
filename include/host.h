#ifndef PYSCHEDULE_HOST_H_
#define PYSCHEDULE_HOST_H_

#include <stdio.h>
#include <iostream>
#include <thread>
#include <memory>

#include "include/macros.h"
#include "include/boost_includes.h"
#include "include/message.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

class Host : public std::enable_shared_from_this<Host> {

  using shared_socket_t = std::shared_ptr<boost::asio::ip::tcp::socket>;

 public:
  explicit Host(int host_id, boost::asio::io_service& service);
  ~Host() {};

  void Run();

 private:
  void SendMessage(Message& msg);
  void RecvMessage(shared_socket_t socket,
    boost::system::error_code const& error);

  int host_id_;
  short port_;

  std::vector<std::thread> thread_pool_;
  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;

  DISALLOW_COPY_AND_ASSIGN(Host);
};


#endif // PYSCHEDULE_HOST_H_
