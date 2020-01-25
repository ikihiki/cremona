#include <net/netlink.h>
#include <net/sock.h>

#include "header.h"

DEFINE_MUTEX(netlink_mutex);

static struct sock *nl_sock;

int recive_create_conn(struct sk_buff *skb, struct nlmsghdr *nlh,
                       struct netlink_ext_ack *ack) {
  char name[10];
  char *data;
  data = (char *)nlmsg_data(nlh);
  int res = getNameFromCreateAccountMessage(data, nlh->nlmsg_len, name, 10);
  if (res == -1) {
    printk(KERN_ERR "Cremona: %s: Cannot decode data \n", __func__);
    return CRMNA_RCV_CREATECONN_FAIL_DECODE;
  }
  res = create_connection(name, nlh->nlmsg_pid);
  if (res < 1) {
    return res;
  }
  for (int i = 0; i < 4; i++)
    ack->cookie[3 - i] = (res >> (i * 8));
  ack->cookie_len = 4;
  printk(KERN_INFO "Cremona: %s: Create device %s", __func__, name);
  return 0;
}

int reciv_new_toot_result(struct sk_buff *skb, struct nlmsghdr *nlh,
                          struct netlink_ext_ack *ack) {
  char *data;
  data = (char *)nlmsg_data(nlh);
  new_toot_result_t result;
  int err = deserialize_new_toot_result(data, nlh->nlmsg_len, &result);
  if (err != 0) {
    return -1;
  }
  notify_open_toot_result(&result);
  return 0;
}

int reciv_add_toot_str_result(struct sk_buff *skb, struct nlmsghdr *nlh,
                              struct netlink_ext_ack *ack) {
  char *data;
  data = (char *)nlmsg_data(nlh);
  add_toot_str_result_t result;
  int err = deserialize_add_toot_str_result(data, nlh->nlmsg_len, &result);
  if (err != 0) {
    return -1;
  }
  notify_add_toot_str_result(&result);
  return 0;
}

int reciv_send_toot_result(struct sk_buff *skb, struct nlmsghdr *nlh,
                           struct netlink_ext_ack *ack) {
  char *data;
  data = (char *)nlmsg_data(nlh);
  send_toot_result_t result;
  int err = deserialize_send_toot_result(data, nlh->nlmsg_len, &result);
  if (err != 0) {
    return -1;
  }
  notify_send_toot_result(&result);
  return 0;
}

int my_rcv_msg(struct sk_buff *skb, struct nlmsghdr *nlh,
               struct netlink_ext_ack *ack) {
  int type;
  char *data;

  type = nlh->nlmsg_type;
  printk(KERN_ERR "Cremona: %s: Reciv Message %d", __func__, type);

  switch (type) {
  case CRMNA_CREATE_CONN:
    return recive_create_conn(skb, nlh, ack);
  case CRMNA_NEW_TOOT_RESULT:
    return reciv_new_toot_result(skb, nlh, ack);
  case CRMNA_ADD_TOOT_STR_RESULT:
    return reciv_add_toot_str_result(skb, nlh, ack);
  case CRMNA_SEND_TOOT_RESULT:
    return reciv_send_toot_result(skb, nlh, ack);

  default:
    printk("%s: unkown message type got %#x\n", __func__, type);
    return -EINVAL;
  }
}

static void recive_message(struct sk_buff *skb) {
  mutex_lock(&netlink_mutex);
  netlink_rcv_skb(skb, &my_rcv_msg);
  mutex_unlock(&netlink_mutex);
  // struct nlmsghdr *nlh;
  // struct sk_buff *skb_out;
  // int msg_size, pid;
  // char *data;
  // char name[10];
  // char result[100];
  // // receiving from user process
  // nlh = nlmsg_hdr(skb);
  // data = (char *)nlmsg_data(nlh);
  // pid = nlh->nlmsg_pid;
  // printk(
  //     KERN_INFO
  //     "received netlink message(%d){ %02x %02x %02x %02x %02x } from
  //     PID(%d)\n", nlh->nlmsg_len, data[0], data[1], data[2], data[3],
  //     data[4], pid);

  // if (!getNameFromCreateAccountMessage(data, nlh->nlmsg_len, name, 10)) {
  //   printk(KERN_INFO "name is %s \n", name);
  // }
  // msg_size = writeCreateAccountResultMessage(10, name, result, 100);
  // if (msg_size != -1) {
  //   skb_out = nlmsg_new(msg_size, 0);
  //   nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
  //   NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
  //   strncpy(nlmsg_data(nlh), result, msg_size);
  //   nlmsg_unicast(nl_sock, skb_out, pid);
  // }
}

int create_link(struct link_data *data, int channel) {
  data->config.input = &recive_message;
  nl_sock = data->socket =
      netlink_kernel_create(&init_net, channel, &(data->config));
  return data->socket != NULL;
}

int delete_link(struct link_data *data) {
  netlink_kernel_release(data->socket);
  data->socket = NULL;
  data->config.input = NULL;
  return true;
}

int send_message(struct link_data *data, u32 pid, int type, char *msg,
                 int msg_size) {
  struct nlmsghdr *nlh;
  struct sk_buff *skb_out;
  printk(KERN_ERR "Cremona: %s: send message to %d, as %d count %d\n", __func__,
         pid, type, msg_size);

  skb_out = nlmsg_new(msg_size, 0);
  nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE | type, msg_size, 0);
  NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
  memcpy(nlmsg_data(nlh), msg, msg_size);
  nlmsg_unicast(data->socket, skb_out, pid);
  return 0;
}
