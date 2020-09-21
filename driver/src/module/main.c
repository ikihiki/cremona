#include "../cremona/central_store.h"
#include <linux/module.h>
#include <linux/netlink.h>
char *crmna_prefix;

#define DRIVER_NAME "Cremona-Mastdon"
/* このデバイスドライバで使うマイナー番号の開始番号と個数(=デバイス数) */
static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM = 33; /* マイナー番号は 0 ~ 1 */

store_t *store;

static int netlink_receiver(struct sk_buff *skb, struct nlmsghdr *nlh,
                            struct netlink_ext_ack *ack) {
  DEFINE_ERROR(err);
  DEFINE_CRMNA_BUF_FROM_MEMORY(data, (char *)NLMSG_DATA(nlh), nlh->nlmsg_len);
  action_t action;
  create_action_from_message(nlh->nlmsg_type, nlh->nlmsg_pid, &data, &action, &err);
  return 0;
}

static int cremona_init(void) {
  printk("Hello my module\n");

  store = create_store();

  // if (!create_device_manager_cntext(&context,MINOR_BASE, MINOR_NUM, DRIVER_NAME, "cremona", 17)) {
  //   printk("Error in create device manager context.\n");
  //   return -ENOENT;
  // }
  return 0;
}

static void cremona_exit(void) {
  //destroy_device_manager_context(&context);
  printk("Bye bye my module\n");
}



module_param(crmna_prefix, charp, 0);

module_init(cremona_init);
module_exit(cremona_exit);

MODULE_LICENSE("Dual BSD/GPL");
