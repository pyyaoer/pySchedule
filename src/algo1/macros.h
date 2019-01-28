#ifndef PYSCHEDULE_MACROS_H_
#define PYSCHEDULE_MACROS_H_

#define PNODE_NUM 1
#define GATE_NUM 1
#define USER_NUM 16
#define TENENT_NUM 3

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

#define THREADS_PER_GATE 10
#define TENENT_LIMIT 100
#define TENENT_RESERVATION 50

#endif //PYSCHEDULE_MACROS_H_
