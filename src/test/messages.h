#ifndef PYSCHEDULE_TEST_MESSAGES_H_
#define PYSCHEDULE_TEST_MESSAGES_H_

#include "include/message.h"

enum eMessageType{ BASIC = 0 };

typedef struct {
  char data[7];
} BasicData;

// Message with at most 7 characters
class BasicMessage: public Message {
  DERIVED_CLASS_PREREQUISITES(BasicMessage)

 public:
  explicit BasicMessage(short src_port, short dst_port)
    : Message(src_port, dst_port, BASIC, sizeof(BasicData)) {}
};

#endif // PYSCHEDULE_TEST_MESSAGES_H_
