#ifndef PYSCHEDULE_BASIC_MESSAGE_H_
#define PYSCHEDULE_BASIC_MESSAGE_H_

#include "include/message.h"

enum eMessageType{ BASIC = 0 };

typedef struct {
  char data[7];
} BasicData;

// Message with at most 7 characters
class BasicMessage: public Message {
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & boost::serialization::base_object<Message>(*this);
  }

 public:
  BasicMessage() {}
  explicit BasicMessage(short src_port, short dst_port)
    : Message(src_port, dst_port, BASIC, sizeof(BasicData)) {}
};

#endif // PYSCHEDULE_BASIC_MESSAGE_H_
