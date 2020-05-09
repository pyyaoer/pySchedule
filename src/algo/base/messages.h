#ifndef PYSCHEDULE_ALGO_BASE_MESSAGES_H_
#define PYSCHEDULE_ALGO_BASE_MESSAGES_H_

#include "include/message.h"

enum eMessageType{ eRequest = 10, eTag, eScheduled, eComplete };

// Message: Request
// Gate g accepts an I/O request from User (belongs to tenant t)
// User -> Gate -> PNode
typedef struct {
  int id; // Message ID
  int user; // User ID
  int tenant; // Tenant ID
  int gate; // Gate ID
  int hardness; // Execution time
  int request_gid = 0; // ID from Gate's view
} RequestData;

class RequestMessage: public Message {
  DERIVED_CLASS_PREREQUISITES(Request);
};

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

// Message: Complete
// Gate g finishes the request from User
// Gate -> User
typedef struct {
  int id;
  int tenant;
  int gate;
  int status;
} CompleteData;

class CompleteMessage : public Message {
  DERIVED_CLASS_PREREQUISITES(Complete);
};


#endif // PYSCHEDULE_ALGO_BASE_MESSAGES_H_

