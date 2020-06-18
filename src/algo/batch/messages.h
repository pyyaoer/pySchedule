#ifndef PYSCHEDULE_ALGO_BATCH_MESSAGES_H_
#define PYSCHEDULE_ALGO_BATCH_MESSAGES_H_

#include "include/message.h"
#include "macros.h"

enum eMessageType{ eSync, eTags };

// Message: Sync
// Gate g syncs requests scheduled in the last period with PNode
// Item ti: scheduled num[t] requests from tenant t with method i
// -1 means unavailable
// Gate -> PNode
typedef struct {
  int gate;
  int period;
  int rnum[MAX_TENANT];
  int dnum[MAX_TENANT];
} SyncData;

class SyncMessage : public Message {
  DERIVED_CLASS_PREREQUISITES(Sync);
};

// Message: Tags
// PNode gives back the parameters for the next period
// -1 means unavailable
// PNode -> Gate
typedef struct {
  double rho[MAX_TENANT];
  double delta[MAX_TENANT];
} TagsData;

class TagsMessage: public Message {
  DERIVED_CLASS_PREREQUISITES(Tags);
};

#endif // PYSCHEDULE_ALGO_BATCH_MESSAGES_H_

