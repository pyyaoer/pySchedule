#ifndef PYSCHEDULE_ALGO1_MESSAGES_H_
#define PYSCHEDULE_ALGO1_MESSAGES_H_

#include "include/message.h"

enum eMessageType{ eRequest = 10, eComplete, eActive };

// Message: Request
// Gate g accepts an I/O request from User (belongs to tenant t)
// User -> Gate -> PNode
typedef struct {
  int id; // Message ID
  int user; // User ID
  int tenant; // Tenant ID
  int gate; // Gate ID
  int hardness; // Execution time
} RequestData;

class RequestMessage: public Message {
  DERIVED_CLASS_PREREQUISITES(Request);
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

// Message: Active
// Gate g is allowed to process a request from tenant t
// PNode -> Gate
typedef struct {
  int tenant;
} ActiveData;

class ActiveMessage : public Message {
  DERIVED_CLASS_PREREQUISITES(Active);
};


#endif // PYSCHEDULE_ALGO1_MESSAGES_H_
