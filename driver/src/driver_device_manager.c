#include "header.h"
#ifdef __KERNEL__

static device_manager_context_t *_context;

static int my_rcv_msg2(struct sk_buff *skb, struct nlmsghdr *nlh,
                       struct netlink_ext_ack *ack)
{
    char error_msg[100];
    crmna_err_t err = {.error_msg = error_msg,
                       .error_msg_len = sizeof(error_msg)};

    // bool result =
    //     reciveMessage(&_context->cremona, nlh->nlmsg_pid, nlh->nlmsg_type,
    //                   (char *)nlmsg_data(nlh), nlh->nlmsg_len, &err);
    // return result ? 0 : 1;
    return 0;
}

static void recive_data(struct sk_buff *skb) {
  netlink_rcv_skb(skb, &my_rcv_msg2);
}

static int crmna_send_message(cremona_device_manager_t *device_manager,
                              uint32_t pid, int type, char *msg,
                              size_t msg_size) {
  struct nlmsghdr *nlh;
  struct sk_buff *skb_out;
  device_manager_context_t *context =
      container_of(device_manager, device_manager_context_t, cremona);

  printk(KERN_INFO "Cremona: %s: send message to %d, as %d count %ld in %p\n",
         __func__, pid, type, msg_size, msg);
  size_t netlink_size =
      msg_size > NLMSG_DEFAULT_SIZE ? msg_size : NLMSG_DEFAULT_SIZE;
  skb_out = nlmsg_new(netlink_size, GFP_KERNEL);
  nlh = nlmsg_put(skb_out, pid, 0, type, netlink_size, 0);
  NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */

  memcpy(nlmsg_data(nlh), msg, msg_size);
  nlmsg_unicast(context->nl_sock, skb_out, pid);
  return 0;
}

static uint32_t get_time(void) { return (uint32_t)jiffies; }
static uint32_t get_rand(void) { return get_random_u32(); }
static void crmna_log(cremona_error_level_t level, char *fmt, ...) {
  va_list ap;
  // 可変長引数を１個の変数にまとめる
  va_start(ap, fmt);
  // まとめられた変数で処理する
  char text[500];
  vsnprintf(text, sizeof(text), fmt, ap);
  switch (level) {
  case ERROR:
    printk(KERN_ERR "%s", text);
    break;
  case WARN:
    printk(KERN_WARNING "%s", text);
    break;
  case INFO:
    printk(KERN_INFO "%s", text);
    break;

  default:
    printk("%s", text);
    break;
  }
  va_end(ap);
}

static void device_manager_lock(cremona_device_manager_t *manager) {
  device_manager_context_t *c =
      container_of(manager, device_manager_context_t, cremona);
  mutex_lock(&(c->mutex));
}

static void device_manager_unlock(cremona_device_manager_t *manager) {
  device_manager_context_t *c =
      container_of(manager, device_manager_context_t, cremona);
  mutex_unlock(&(c->mutex));
}

static bool rent_miner_num(cremona_device_manager_t *device_manager,
                           unsigned int *miner) {
  printk(KERN_ERR "Cremona: dev: rent_miner_num %p  %p\n", device_manager, miner);
  unsigned int result;
  device_manager_context_t *c =
      container_of(device_manager, device_manager_context_t, cremona);
  
  printk(KERN_ERR "Cremona: dev: rent_miner_num %p  %p %p\n", device_manager, miner, c);

  result = ffs(~c->miner_list);
  printk(KERN_ERR "Cremona: dev: rent_miner_num %p  %p %p %d\n", device_manager,
         miner, c, result);
  if (result != 0) {
    c->miner_list = c->miner_list | (1 << (result - 1));
    *miner = result;
    return true;
  }
  return false;
}
static void release_miner_num(cremona_device_manager_t *device_manager, unsigned int miner) {
  device_manager_context_t *c =
      container_of(device_manager, device_manager_context_t, cremona);

  c->miner_list = c->miner_list | (1 << (miner - 1));
}

void set_device_manager_callbacks(
    cremona_device_manager_callbacks_t *callbacks) {
  callbacks->send_message = &crmna_send_message;
  callbacks->lock = &device_manager_lock;
  callbacks->unlock = &device_manager_unlock;
  callbacks->get_rand = &get_rand;
  callbacks->get_time = &get_time;
  callbacks->log = &crmna_log;
  callbacks->rent_miner_num = &rent_miner_num;
  callbacks->release_miner_num = &release_miner_num;
}

bool create_device_manager_cntext(device_manager_context_t *context,
                                  unsigned int miner_base,
                                  unsigned int miner_max,
                                  const char *driver_name,
                                  const char *class_name, int nl_unit) {
  int alloc_ret = 0;
  dev_t dev;
  struct class *device_class;
  struct sock *nl_sock;

  /* 1. 空いているメジャー番号を確保する */
  alloc_ret = alloc_chrdev_region(&dev, miner_base, miner_max, driver_name);
  if (alloc_ret != 0) {
    printk(KERN_ERR "alloc_chrdev_region = %d\n", alloc_ret);
    return -1;
  }
  context->miner_base = miner_base;
  context->miner_max = miner_max;
  context->major_num = MAJOR(dev);

  /* 5. このデバイスのクラス登録をする(/sys/class/mydevice/ を作る) */
  device_class = class_create(THIS_MODULE, class_name);
  if (IS_ERR(device_class)) {
    printk(KERN_ERR "class_create\n");
    unregister_chrdev_region(dev, miner_max);
    return false;
  }
  context->device_class = device_class;

  context->nl_config.input = &recive_data;
  nl_sock = netlink_kernel_create(&init_net, nl_unit, &context->nl_config);

  if (!nl_sock) {
    printk(KERN_ERR "cant create link\n");
    class_destroy(device_class);
    unregister_chrdev_region(dev, miner_max);
    return false;
  }
  context->nl_sock = nl_sock;
  context->nl_unit = nl_unit;

  set_device_manager_callbacks(
      &context->cremona.config.devicce_manager_callbacks);
  //set_device_callbacks(&context->cremona.config.device_collbacks);
  set_toot_callbacks(&context->cremona.config.toot_callbacks);

  mutex_init(&(context->mutex));

  //init_device_manager(&context->cremona);
  _context = context;
}

void destroy_device_manager_context(device_manager_context_t *context) {

  //destroy_device_manager(&context->cremona);
  mutex_destroy(&(context->mutex));

  class_destroy(context->device_class);
  dev_t dev = MKDEV(context->major_num, context->miner_base);

  /* 6. このデバイスドライバで使用していたメジャー番号の登録を取り除く */
  unregister_chrdev_region(dev, context->miner_max);

  netlink_kernel_release(context->nl_sock);
  _context = NULL;
}

#endif