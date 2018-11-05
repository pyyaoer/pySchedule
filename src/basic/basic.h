#ifndef PYSCHEDULE_BASIC_H_
#define PYSCHEDULE_BASIC_H_

#include "include/message.h"

enum eMessageType{ REQUEST = 0, COMPLETE };

class RequestMessage : public Message {
 public:
  explicit RequestMessage(const char* data) : Message(data) {
    sscanf(data, "%d %d %d %lld %d", &src_port_, &dst_port_, &type_, &create_time_, &hardness_);
  };
  explicit RequestMessage(short src_port, short dst_port, int hardness)
    : Message(src_port, dst_port, REQUEST), hardness_(hardness) {}

  int GetHardness() { return hardness_; }
  std::string ToString() { 
    std::stringstream ret;
    ret << src_port_ << " " << dst_port_ << " " << type_ 
        << " " << create_time_ << " " << hardness_ << "\0";
    return ret.str();
  }

 private:
  int hardness_;
  
};
 
class CompleteMessage : public Message {
 public:
  explicit CompleteMessage(short src_port, short dst_port)
    : Message(src_port, dst_port, COMPLETE) {}

 private:
  
};

#endif //PYSCHEDULE_BASIC_H_
