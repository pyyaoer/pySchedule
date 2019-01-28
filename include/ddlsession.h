#ifndef PYSCHEDULE_DDLSESSION_H_
#define PYSCHEDULE_DDLSESSION_H_

#include "include/lib_include.h"

struct DDLSession : boost::enable_shared_from_this<DDLSession> {
  explicit DDLSession(boost::asio::io_service& io_service, std::function<void()> handler, long long wait) :
    timer_(io_service, boost::posix_time::milliseconds(wait)), handler_(handler) {}

  void start() {
    timer_.async_wait(boost::bind(&DDLSession::handler, shared_from_this(), boost::asio::placeholders::error));
  }

private:
  void handler(const boost::system::error_code &ec) {
    if(ec) {
      std::cout << "Error in DDLSession: " << ec.message() << std::endl;
      exit(1);
    }
    handler_();
  }
  std::function<void()> handler_;
  boost::asio::deadline_timer timer_;

  DISALLOW_COPY_AND_ASSIGN(DDLSession);
};

#endif // PYSCHEDULE_DDLSESSION_H_
