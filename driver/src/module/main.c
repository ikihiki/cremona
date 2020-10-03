#include "../cremona/central_store.h"
#include "module.h"
#include "net_link_comm.h"
#include <linux/module.h>
#include <linux/netlink.h>
char *crmna_prefix;

#define DRIVER_NAME "Cremona-Mastdon"
/* このデバイスドライバで使うマイナー番号の開始番号と個数(=デバイス数) */
static const uint32_t MINOR_BASE = 0;
static const uint32_t MINOR_NUM = 33; /* マイナー番号は 0 ~ 1 */

store_t *store;
communicator_ref_t com;

static int cremona_init(void) {
  printk("Hello my module\n");

  store = create_store(MINOR_BASE, MINOR_NUM, DRIVER_NAME);
  if (store == NULL) {
    return -EFAULT;
  }
  if (!create_communicator_and_register_store(17, store, &com)) {
    destroy_store(store);
    return -EFAULT;
  }
  return 0;
}

static void cremona_exit(void) {
  destroy_store(store);
  communicator_free(&com);
  printk("Bye bye my module\n");
}

module_param(crmna_prefix, charp, 0);

module_init(cremona_init);
module_exit(cremona_exit);

MODULE_LICENSE("Dual BSD/GPL");
