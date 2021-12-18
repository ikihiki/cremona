#include <linux/module.h>
#include <linux/netlink.h>

#define DRIVER_NAME "Cremona-Mastdon"




static int cremona_init(void) {
  printk("Hello ceremona module\n");
  return 0;
}

static void cremona_exit(void) {
  printk("Bye bye cremona module\n");
}

module_init(cremona_init);
module_exit(cremona_exit);

MODULE_LICENSE("Dual BSD/GPL");
