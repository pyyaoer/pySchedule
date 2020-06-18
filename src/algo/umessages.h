#ifndef PYSCHEDULE_ALGO_UMESSAGES_H_
#define PYSCHEDULE_ALGO_UMESSAGES_H_

#include "include/message.h"

enum eUserMessageType{ eRequest = 0x100, eComplete };

// Message: Request
// Gate g accepts an I/O request from User (belongs to tenant t)
// User -> Gate
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

#endif // PYSCHEDULE_ALGO_UMESSAGES_H_
