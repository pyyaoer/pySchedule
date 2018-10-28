#ifndef PYSCHEDULE_MACROS_H_
#define PYSCHEDULE_MACROS_H_

#define PNODE_NUM 1
#define HOST_NUM 4
#define USER_NUM 16

#define PNODE_SEND_BASE 20000
#define HOST_SEND_BASE (PNODE_SEND_BASE + PNODE_NUM)
#define USER_SEND_BASE (HOST_SEND_BASE + HOST_NUM)

#define PNODE_RECV_BASE 30000
#define HOST_RECV_BASE (PNODE_RECV_BASE + PNODE_NUM)
#define USER_RECV_BASE (HOST_RECV_BASE + HOST_NUM)

#define HOST "127.0.0.1"

#define MESSAGE_SIZE_MAX 1024

#endif //PYSCHEDULE_MACROS_H_

