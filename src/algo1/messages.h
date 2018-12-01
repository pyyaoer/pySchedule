#ifndef PYSCHEDULE_ALGO1_MESSAGES_H_
#define PYSCHEDULE_ALGO1_MESSAGES_H_

#include "include/message.h"

enum eMessageType{ eRequest = 0, eComplete, eActive };

// Message: Request
// Gate g accepts an I/O request from User (belongs to tenent t)
// User -> Gate -> PNode
typedef struct {
  int tenent;
  int gate;
  int hardness;
} RequestData;

class RequestMessage: public Message {
  DERIVED_CLASS_PREREQUISITES(Request);
};

// Message: Complete
// Gate g finishes the request from User
// Gate -> User
typedef struct {
  int status;
} CompleteData;

class CompleteMessage : public Message {
  DERIVED_CLASS_PREREQUISITES(Complete);
};

// Message: Active
// Gate g is allowed to process a request from tenent t
typedef struct {
  int tenent;
} ActiveData;

class ActiveMessage : public Message {
  DERIVED_CLASS_PREREQUISITES(Active);
};


#endif // PYSCHEDULE_ALGO1_MESSAGES_H_
