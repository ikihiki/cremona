#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/netlink.h>
#include <linux/uaccess.h>
#include <net/sock.h>

#include "cremona.h"

#define DEV

char *crmna_prefix;

static struct sock *nl_sock;
static struct netlink_kernel_cfg config;

#define DRIVER_NAME "Cremona-Mastdon"
/* このデバイスドライバで使うマイナー番号の開始番号と個数(=デバイス数) */
static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM = 33; /* マイナー番号は 0 ~ 1 */

/* このデバイスドライバのメジャー番号(動的に決める) */
unsigned int mydevice_major;

/* キャラクタデバイスのオブジェクト */
struct class *mydevice_class = NULL;

cremona_device_manager_t device_manager;

static int my_rcv_msg2(struct sk_buff *skb, struct nlmsghdr *nlh,
                       struct netlink_ext_ack *ack) {
  reciveMessage(&device_manager, nlh->nlmsg_pid, nlh->nlmsg_type,
                (char *)nlmsg_data(nlh), nlh->nlmsg_len);
}

static void recive_data(struct sk_buff *skb) {
  netlink_rcv_skb(skb, &my_rcv_msg2);
}

static int crma_send_message(uint32_t pid, int type, char *msg,
                             size_t msg_size) {
  struct nlmsghdr *nlh;
  struct sk_buff *skb_out;

  printk(KERN_INFO "Cremona: %s: send message to %d, as %d count %d in %p\n",
         __func__, pid, type, msg_size, msg);
  size_t netlink_size =
      msg_size > NLMSG_DEFAULT_SIZE ? msg_size : NLMSG_DEFAULT_SIZE;
  skb_out = nlmsg_new(netlink_size, GFP_KERNEL);
  nlh = nlmsg_put(skb_out, pid, 0, NLMSG_DONE | type, netlink_size, 0);
  NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */

  memcpy(nlmsg_data(nlh), msg, msg_size);
  nlmsg_unicast(nl_sock, skb_out, pid);
  return 0;
}

typedef struct device_manager_context {
  struct mutex mutex;
} device_manager_context_t;

device_manager_context_t context;

void device_manager_lock(cremona_device_manager_t *manager) {
  device_manager_context_t *c = (device_manager_context_t*)manager->context;
  mutex_lock(&(c->mutex));
}

void device_manager_unlock(cremona_device_manager_t *manager) {
  device_manager_context_t *c = (device_manager_context_t*)manager->context;
  mutex_unlock(&(c->mutex));
}

typedef struct device_context {
  struct mutex mutex;
} device_context_t;

typedef struct toot_context {
  wait_queue_head_t open_wait;
} toot_context_t;

static int cremona_init(void) {
  printk("Hello my module\n");

  // if(!create_link(&link, 0x10 +2))
  // {
  //     printk("Error in create link\n");
  //     return -ENOENT;
  // }
  int alloc_ret = 0;
  dev_t dev;
  config.input = &recive_data;
  nl_sock = netlink_kernel_create(&init_net, 17, &config);

  if (!nl_sock) {
    printk(KERN_ERR "cant create link\n");
    return -1;
  }

  /* 1. 空いているメジャー番号を確保する */
  alloc_ret = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUM, DRIVER_NAME);
  if (alloc_ret != 0) {
    printk(KERN_ERR "alloc_chrdev_region = %d\n", alloc_ret);
    return -1;
  }

  /* 2. 取得したdev( = メジャー番号 +
   * マイナー番号)からメジャー番号を取得して保持しておく */
  mydevice_major = MAJOR(dev);
  dev = MKDEV(mydevice_major, MINOR_BASE); /* 不要? */

  /* 5. このデバイスのクラス登録をする(/sys/class/mydevice/ を作る) */
  mydevice_class = class_create(THIS_MODULE, "mydevice");
  if (IS_ERR(mydevice_class)) {
    printk(KERN_ERR "class_create\n");
    unregister_chrdev_region(dev, MINOR_NUM);
    return -1;
  }
  device_manager.config.devicce_manager_callbacks.send_message =
      &crma_send_message;
  device_manager.config.devicce_manager_callbacks.lock =
      &device_manager_lock;
  device_manager.config.devicce_manager_callbacks.unlock =
      &device_manager_unlock;

  device_manager.context = &context;
  mutex_init(&(context.mutex));

  initDeviceManager(&device_manager);

  char word[] = "hello world!";
  struct sk_buff *skb;
  int type = 111;
  struct nlmsghdr *nlh;
  char *d;
  int ret;

  skb = alloc_skb(1024, GFP_ATOMIC);
  if (!skb) {
    printk("error: skb alloc\n");
    return;
  }

  nlh = nlmsg_put(skb, 0, 0, type, strlen(word), 0);
  d = nlmsg_data(nlh);
  memcpy(d, word, strlen(word));

  ret = netlink_broadcast(nl_sock, skb, 0, 5, GFP_ATOMIC);

  return 0;
}

static void cremona_exit(void) {
  printk("Bye bye my module\n");

  class_destroy(mydevice_class);
  dev_t dev = MKDEV(mydevice_major, MINOR_BASE);

  /* 6. このデバイスドライバで使用していたメジャー番号の登録を取り除く */
  unregister_chrdev_region(dev, MINOR_NUM);

  netlink_kernel_release(nl_sock);
}

module_param(crmna_prefix, charp, 0);

module_init(cremona_init);
module_exit(cremona_exit);

MODULE_LICENSE("Dual BSD/GPL");
