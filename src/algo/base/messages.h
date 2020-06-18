#ifndef PYSCHEDULE_ALGO_BASE_MESSAGES_H_
#define PYSCHEDULE_ALGO_BASE_MESSAGES_H_

#include "include/message.h"

enum eMessageType{ eTag, eScheduled };

// Message: Tag
// Gate g should assign the request with the given rho and delta
// PNode -> Gate
typedef struct {
  int rho;
  int delta;
  int request_gid;
} TagData;

class TagMessage: public Message {
  DERIVED_CLASS_PREREQUISITES(Tag);
};

// Message: Scheduled
// Gate g scheduled a request from tenant t with method b
// Gate -> PNode
typedef struct {
  int tenant;
  int gate;
  int method; // 0 or 1
} ScheduledData;

class ScheduledMessage : public Message {
  DERIVED_CLASS_PREREQUISITES(Scheduled);
};

#endif // PYSCHEDULE_ALGO_BASE_MESSAGES_H_

