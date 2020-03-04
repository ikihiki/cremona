#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/netlink.h>
#include <linux/random.h>
#include <linux/time.h>
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

typedef struct device_manager_context {
  struct mutex mutex;
  cremona_device_manager_t cremona;
} device_manager_context_t;

device_manager_context_t context;

static int my_rcv_msg2(struct sk_buff *skb, struct nlmsghdr *nlh,
                       struct netlink_ext_ack *ack) {
  char error_msg[100];
  crmna_err_t err = {.error_msg = error_msg,
                     .error_msg_len = sizeof(error_msg)};
  bool result = reciveMessage(&context.cremona, nlh->nlmsg_pid, nlh->nlmsg_type,
                              (char *)nlmsg_data(nlh), nlh->nlmsg_len, &err);
  return result ? 0 : 1;
}

static void recive_data(struct sk_buff *skb) {
  netlink_rcv_skb(skb, &my_rcv_msg2);
}

static int crmna_send_message(uint32_t pid, int type, char *msg,
                              size_t msg_size) {
  struct nlmsghdr *nlh;
  struct sk_buff *skb_out;

  printk(KERN_INFO "Cremona: %s: send message to %d, as %d count %ld in %p\n",
         __func__, pid, type, msg_size, msg);
  size_t netlink_size =
      msg_size > NLMSG_DEFAULT_SIZE ? msg_size : NLMSG_DEFAULT_SIZE;
  skb_out = nlmsg_new(netlink_size, GFP_KERNEL);
  nlh = nlmsg_put(skb_out, pid, 0, type, netlink_size, 0);
  NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */

  memcpy(nlmsg_data(nlh), msg, msg_size);
  nlmsg_unicast(nl_sock, skb_out, pid);
  return 0;
}

uint32_t get_time(void) { return (uint32_t)jiffies; }
uint32_t get_rand(void) { return get_random_u32(); }
void crmna_log(cremona_error_level_t level, char *fmt, ...) {
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

void device_manager_lock(cremona_device_manager_t *manager) {
  device_manager_context_t *c =
      container_of(manager, device_manager_context_t, cremona);
  mutex_lock(&(c->mutex));
}

void device_manager_unlock(cremona_device_manager_t *manager) {
  device_manager_context_t *c =
      container_of(manager, device_manager_context_t, cremona);
  mutex_unlock(&(c->mutex));
}

static unsigned long miner_list = 0;
DEFINE_SPINLOCK(miner_list_lock);
static void release_flag(int number) {
  spin_lock(&miner_list_lock);
  miner_list = miner_list | (1 << (number - 1));
  spin_unlock(&miner_list_lock);
}
static int check_flag(int number) {
  int flag;
  spin_lock(&miner_list_lock);
  flag = miner_list & (1 << (number - 1));
  spin_unlock(&miner_list_lock);
  return flag;
}

typedef struct toot_context {
  struct mutex mutex;
  wait_queue_head_t wait_head;
  cremona_toot_t cremona;

} toot_context_t;

void crmna_toot_lock(cremona_toot_t *toot) {
  toot_context_t *context = container_of(toot, toot_context_t, cremona);
  mutex_lock(&context->mutex);
}
void crmna_toot_unlock(cremona_toot_t *toot) {
  toot_context_t *context = container_of(toot, toot_context_t, cremona);
  mutex_unlock(&context->mutex);
}
void crmna_toot_wait(cremona_toot_t *toot, toot_wait_type_t wait_type) {
  toot_context_t *context = container_of(toot, toot_context_t, cremona);
  printk(KERN_INFO "Cremona: %s: wait type: %d toot id: %llu\n", __func__,
         wait_type, toot->id);

  switch (wait_type) {
  case WAIT_OPEN:
    wait_event_interruptible_timeout(context->wait_head,
                                     context->cremona.state == OPEND ||
                                         context->cremona.state == DESTROYED ||
                                         context->cremona.state == ERROR,
                                     10 * HZ / 1000);
    break;
    case WAIT_CLOSE:
      wait_event_interruptible_timeout(context->wait_head,
                                           context->cremona.state ==
                                               DESTROYED ||
                                           context->cremona.state == ERROR,
                                       10 * HZ / 1000);
      break;
    default:
      break;
  }
  printk(KERN_INFO "Cremona: %s: wake type: %d toot id: %llu\n", __func__,
         wait_type, toot->id);
}
void crmna_toot_notify(cremona_toot_t *toot) {
  printk(KERN_INFO "Cremona: %s: notify state: %d toot id: %llu\n", __func__,
         toot->state, toot->id);
  toot_context_t *context = container_of(toot, toot_context_t, cremona);
  wake_up_interruptible(&context->wait_head);
}
cremona_toot_t *crmna_toot_create_toot(void) {
  toot_context_t *context = kmalloc(sizeof(toot_context_t), GFP_KERNEL);
  if (context == NULL) {
    return NULL;
  }
  mutex_init(&context->mutex);
  init_waitqueue_head(&context->wait_head);

  return &context->cremona;

err:
  kfree(context);
  return NULL;
}
bool crmna_toot_destroy_toot(cremona_toot_t *toot) { return true; }
void crmna_toot_cleanup_toot(cremona_toot_t *toot) {
  toot_context_t *context = container_of(toot, toot_context_t, cremona);

  mutex_destroy(&context->mutex);
  kfree(context);
}

typedef struct device_context {
  struct mutex mutex;
  struct device device;
  struct cdev cdev;
  cremona_device_t cremona;
} device_context_t;

int toot_open(struct inode *inode, struct file *file) {
  printk(KERN_INFO "Cremona: %s: toot open\n", __func__);

  toot_context_t *toot_context;

  if (file->private_data != NULL) {
    toot_context = (toot_context_t *)file->private_data;
  } else {

    char error_msg[100];
    crmna_err_t err = {.error_msg = error_msg,
                       .error_msg_len = sizeof(error_msg)};

    bool wait = (file->f_flags &O_NONBLOCK) == 0;
    device_context_t *device_context =
        container_of(inode->i_cdev, device_context_t, cdev);
    cremona_toot_t *toot = open_toot(&device_context->cremona, wait, &err);
    toot_context = container_of(toot, toot_context_t, cremona);
    file->private_data = (void *)toot_context;
  }
  mutex_lock(&toot_context->mutex);
  int result = -EIO;
  if (toot_context->cremona.state == OPEND) {
    result = 0;
  }
  if (toot_context->cremona.state == OPEN_RESULT_WAIT) {
    result = -EAGAIN;
  }
  mutex_unlock(&toot_context->mutex);
  printk(KERN_INFO "Cremona: %s: toot open compleate\n", __func__);

  return result;
}

int toot_close(struct inode *inode, struct file *file) {
  printk(KERN_INFO "Cremona: %s: toot close\n", __func__);

  if (file->private_data == NULL) {
    return 0;
  }
  toot_context_t *toot_context = (toot_context_t *)file->private_data;

  if (toot_context->cremona.state == DESTROYED ||
      toot_context->cremona.state == TOOT_ERROR) {
    file->private_data = NULL;
    release_toot(&toot_context->cremona);
    return 0;
  }

  char error_msg[100];
  crmna_err_t err = {.error_msg = error_msg,
                     .error_msg_len = sizeof(error_msg)};

  bool result = close_toot(&toot_context->cremona, &err);
  if (result) {
    file->private_data = NULL;
    release_toot(&toot_context->cremona);
    return 0;
  } else {
    file->private_data = NULL;
    release_toot(&toot_context->cremona);
    return -EIO;
  }
}

static struct file_operations ops = {
    .open = &toot_open, .release = toot_close,
    // .write = &toot_write,
};

static void crmna_device_lock(cremona_device_t *device) {
  device_context_t *context = container_of(device, device_context_t, cremona);
  mutex_lock(&(context->mutex));
}
static void cremona_device_unlock(cremona_device_t *device) {
  device_context_t *context = container_of(device, device_context_t, cremona);
  mutex_unlock(&(context->mutex));
}
cremona_device_t *crmna_create_device(char *name) {
  device_context_t *context = kmalloc(sizeof(device_context_t), GFP_KERNEL);
  if (context == NULL) {
    return NULL;
  }

  mutex_init(&(context->mutex));

  int miner = -1;
  spin_lock(&miner_list_lock);
  miner = ffs(~miner_list);
  if (miner != 0) {
    miner_list = miner_list | (1 << (miner - 1));
  }
  spin_unlock(&miner_list_lock);
  if (miner == 0) {
    // LOG_ERROR(device->device_manager, "Cannot register %s no space\n",
    //           device->name);

    goto err;
  }

  device_initialize(&(context->device));
  context->device.class = mydevice_class;
  context->device.devt = MKDEV(mydevice_major, miner);
  kobject_set_name(&(context->device.kobj), "crmna_%s", name);

  cdev_init(&context->cdev, &ops);
  context->cdev.owner = THIS_MODULE;

  int err = cdev_device_add(&context->cdev, &context->device);
  if (err != 0) {
    // LOG_ERROR(device->device_manager, "Fail add device %s, because %d",
    //           device->name, err);
    goto err;
  }

  if (check_flag(miner) == 0) {
    // LOG_ERROR(device->device_manager, "Cannot register %s as miner: %d\n",
    //           device->name, miner);
    cdev_device_del(&context->cdev, &context->device);
    goto err;
  }
  return &context->cremona;

err:
  kfree(context);
  return NULL;
}
bool crmna_destroy_device(cremona_device_t *device) {
  device_context_t *context = container_of(device, device_context_t, cremona);
  cdev_device_del(&context->cdev, &context->device);
  return true;
}
void crmna_cleanup_device(cremona_device_t *device) {
  device_context_t *context = container_of(device, device_context_t, cremona);
  mutex_destroy(&(context->mutex));
  kfree(context);
}

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
  context.cremona.config.devicce_manager_callbacks.send_message =
      &crmna_send_message;
  context.cremona.config.devicce_manager_callbacks.lock = &device_manager_lock;
  context.cremona.config.devicce_manager_callbacks.unlock =
      &device_manager_unlock;
  context.cremona.config.devicce_manager_callbacks.get_rand = &get_rand;
  context.cremona.config.devicce_manager_callbacks.get_time = &get_time;
  context.cremona.config.devicce_manager_callbacks.log = &crmna_log;

  context.cremona.config.device_collbacks.lock = &crmna_device_lock;
  context.cremona.config.device_collbacks.unlock = &cremona_device_unlock;
  context.cremona.config.device_collbacks.create_device = &crmna_create_device;
  context.cremona.config.device_collbacks.destroy_device =
      &crmna_destroy_device;
  context.cremona.config.device_collbacks.cleanup_device =
      &crmna_cleanup_device;

  context.cremona.config.toot_callbacks.create_toot = &crmna_toot_create_toot;
  context.cremona.config.toot_callbacks.wait = &crmna_toot_wait;
  context.cremona.config.toot_callbacks.notify = &crmna_toot_notify;
  context.cremona.config.toot_callbacks.lock = &crmna_toot_lock;
  context.cremona.config.toot_callbacks.unlock = &crmna_toot_unlock;
  context.cremona.config.toot_callbacks.destroy_toot = &crmna_toot_destroy_toot;
  context.cremona.config.toot_callbacks.cleanup_toot = &crmna_toot_cleanup_toot;

  mutex_init(&(context.mutex));

  init_device_manager(&context.cremona);

  char word[] = "hello world!";
  struct sk_buff *skb;
  int type = 111;
  struct nlmsghdr *nlh;
  char *d;
  int ret;

  skb = alloc_skb(1024, GFP_ATOMIC);
  if (!skb) {
    printk("error: skb alloc\n");
    return ECONNREFUSED;
  }

  nlh = nlmsg_put(skb, 0, 0, type, strlen(word), 0);
  d = nlmsg_data(nlh);
  memcpy(d, word, strlen(word));

  ret = netlink_broadcast(nl_sock, skb, 0, 5, GFP_ATOMIC);

  return 0;
}

static void cremona_exit(void) {
  printk("Bye bye my module\n");

  destroy_device_manager(&context.cremona);
  mutex_destroy(&(context.mutex));

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
