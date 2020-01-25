#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#include "header.h"

#define DEV

char *crmna_prefix;

struct link_data link;

#define DRIVER_NAME "Cremona-Mastdon"
/* このデバイスドライバで使うマイナー番号の開始番号と個数(=デバイス数) */
static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM = 33; /* マイナー番号は 0 ~ 1 */

/* このデバイスドライバのメジャー番号(動的に決める) */
unsigned int mydevice_major;

/* キャラクタデバイスのオブジェクト */
struct class *mydevice_class = NULL;

static int cremona_init(void) {
  printk("Hello my module\n");
  // if(!create_link(&link, 0x10 +2))
  // {
  //     printk("Error in create link\n");
  //     return -ENOENT;
  // }
  int alloc_ret = 0;
  dev_t dev;

  if (!create_link(&link, 17)) {
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

  // /* 6. /sys/class/mydevice/mydevice* を作る */
  // int minor;
  // for (minor = MINOR_BASE; minor < MINOR_BASE + MINOR_NUM; minor++) {
  //   device_create(mydevice_class, NULL, MKDEV(mydevice_major, minor), NULL,
  //                 "mydevice%d", minor);
  // }
  init_devices();
  return 0;
}

static void cremona_exit(void) {
  // if(!delete_link(&link))
  // {
  //     printk("Error in delete link\n");
  // }
  printk("Bye bye my module\n");

  delete_link(&link);
  destroy_all_connections();
  class_destroy(mydevice_class);
  dev_t dev = MKDEV(mydevice_major, MINOR_BASE);

  /* 6. このデバイスドライバで使用していたメジャー番号の登録を取り除く */
  unregister_chrdev_region(dev, MINOR_NUM);
}

module_param(crmna_prefix, charp, 0);

module_init(cremona_init);
module_exit(cremona_exit);

MODULE_LICENSE("Dual BSD/GPL");
