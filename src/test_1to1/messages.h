#ifndef PYSCHEDULE_MESSAGES_H_
#define PYSCHEDULE_MESSAGES_H_

#include "include/message.h"

enum eMessageType{ eBasic = 0 };

typedef struct {
  char data[7];
} BasicData;

// Message with at most 7 characters
class BasicMessage: public Message {
  DERIVED_CLASS_PREREQUISITES(Basic);
};

#endif // PYSCHEDULE_MESSAGES_H_
