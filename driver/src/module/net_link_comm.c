#include "net_link_comm.h"
#include "module.h"
#include <linux/init.h>
#include <linux/netlink.h>
#include <net/sock.h>

typedef struct {
  struct sock *nl_sock;
  store_t *store;
} netlink_communicator_t;

static int process_recive_data(struct sk_buff *skb, struct nlmsghdr *nlh,
                               struct netlink_ext_ack *ack) {
  DEFINE_ERROR(err);
  netlink_communicator_t *com = (netlink_communicator_t *)skb->sk->sk_user_data;
  action_t action;
  DEFINE_CRMNA_BUF_FROM_MEMORY(buf, (char *)nlmsg_data(nlh), nlh->nlmsg_len);
  if (!create_action_from_message(nlh->nlmsg_type, nlh->nlmsg_pid, &buf,
                                  &action, &err)) {
    ADD_ERROR((&err), "recive message err create action");
    printk_err(&err);
    return 1;
  }
  if (!dispatch(com->store, &action, &err))
  {
    ADD_ERROR((&err), "recive message err dispatch");
    printk_err(&err);
    return 1;
  }
  return 0;
}

static void recive_data(struct sk_buff *skb) {
  netlink_rcv_skb(skb, &process_recive_data);
}

struct netlink_kernel_cfg cfg = {
    .input = &recive_data,
};
bool send_message(void *obj, uint32_t pid, int type, crmna_buf_t *buf,
             crmna_err_t *err)
{
  netlink_communicator_t *com = (netlink_communicator_t *)obj;
  struct nlmsghdr *nlh;
  struct sk_buff *skb_out;

  // printk(KERN_INFO "Cremona: %s: send message to %d, as %d count %ld in %p\n ",
  //        __func__, pid, type, buf->used_size, buf->buf);
  size_t netlink_size = buf->used_size;
  skb_out = nlmsg_new(netlink_size, GFP_KERNEL);
  nlh = nlmsg_put(skb_out, pid, 0, type, netlink_size, 0);
  NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */

  memcpy(nlmsg_data(nlh), buf->buf, buf->used_size);
  if(nlmsg_unicast(com->nl_sock, skb_out, pid) < 0){
    ADD_ERROR(err, "send message error");
    return false;
  }
  return true;
}
void free(void *obj) {
  netlink_communicator_t *com = (netlink_communicator_t *)obj;
  netlink_kernel_release(com->nl_sock);
  kfree(com);
}

communicator_t interface = {.send_message = &send_message, .free = &free};

bool create_communicator_and_register_store(int port, store_t *store,
                                            communicator_ref_t *ref) {
  netlink_communicator_t *com =
      kmalloc(sizeof(netlink_communicator_t), GFP_KERNEL);
  if (com == NULL) {
    return false;
  }
  com->nl_sock = netlink_kernel_create(&init_net, port, &cfg);
  if (!com->nl_sock) {
    kfree(com);
    return false;
  }
  com->store = store;
  com->nl_sock->sk_user_data = com;
  ref->obj = com;
  ref->interface = &interface;
  set_communicator(store, *ref);
  return true;
}
