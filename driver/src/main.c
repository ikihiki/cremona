#include "header.h"

#define DEV

char *crmna_prefix;

#define DRIVER_NAME "Cremona-Mastdon"
/* このデバイスドライバで使うマイナー番号の開始番号と個数(=デバイス数) */
static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM = 33; /* マイナー番号は 0 ~ 1 */

device_manager_context_t context;

static int cremona_init(void) {
  printk("Hello my module\n");

  if (!create_device_manager_cntext(&context,MINOR_BASE, MINOR_NUM, DRIVER_NAME, "cremona", 17)) {
    printk("Error in create device manager context.\n");
    return -ENOENT;
  }
  return 0;
}

static void cremona_exit(void) {
  destroy_device_manager_context(&context);
  printk("Bye bye my module\n");
}

module_param(crmna_prefix, charp, 0);

module_init(cremona_init);
module_exit(cremona_exit);

MODULE_LICENSE("Dual BSD/GPL");
