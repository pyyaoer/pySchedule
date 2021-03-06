#ifndef PYSCHEDULE_ALGO_ZERO_MACROS_H_
#define PYSCHEDULE_ALGO_ZERO_MACROS_H_

extern int PNODE_NUM_;
extern int GATE_NUM_;
extern int USER_NUM_;
extern int TENANT_NUM_;
extern int THREADS_PER_GATE_;
extern int TENANT_LIMIT_;
extern int TENANT_RESERVATION_;

#define PNODE_NUM PNODE_NUM_
#define GATE_NUM GATE_NUM_
#define USER_NUM USER_NUM_
#define TENANT_NUM TENANT_NUM_

#define PNODE_ID_START ID_BASE
#define PNODE_ID_END (PNODE_ID_START + PNODE_NUM)
#define GATE_ID_START PNODE_ID_END
#define GATE_ID_END (GATE_ID_START + GATE_NUM)
#define USER_ID_START GATE_ID_END
#define USER_ID_END (USER_ID_START + USER_NUM)

#define GID2NID(gid) (gid + GATE_ID_START)
#define NID2GID(nid) (nid - GATE_ID_START)
#define UID2NID(uid) (gid + USER_ID_START)
#define NID2UID(nid) (uid - USER_ID_START)

#define THREADS_PER_GATE THREADS_PER_GATE_
#define TENANT_LIMIT TENANT_LIMIT_
#define TENANT_RESERVATION TENANT_RESERVATION_

#endif //PYSCHEDULE_ALGO_ZERO_MACROS_H_

