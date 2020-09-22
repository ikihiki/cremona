#include "net_link_comm.h"
#include <linux/init.h>
#include <linux/netlink.h>
#include <net/sock.h>

typedef struct 
{
    struct sock *nl_sock;
} netlink_communicator_t;

struct netlink_kernel_cfg cfg = {
    .input = nl_recv_msg,
};

static void nl_recv_msg(struct sk_buff *skb_in) {}

  communicator_ref_t create_communicator(int port) { 
    struct sock *nl_sock=netlink_kernel_create(&init_net, port, )
     }
