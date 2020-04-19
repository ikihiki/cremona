#ifndef __HEADER__
#define __HEADER__
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

typedef struct device_manager_context {
  struct mutex mutex;
  unsigned long miner_list;
  unsigned int miner_base;
  unsigned int miner_max;
  unsigned int major_num;
  struct class *device_class;
  struct sock *nl_sock;
  struct netlink_kernel_cfg nl_config;
  int nl_unit;
  cremona_device_manager_t cremona;
} device_manager_context_t;

typedef struct toot_context {
  struct mutex mutex;
  wait_queue_head_t wait_head;
  cremona_toot_t cremona;

} toot_context_t;

typedef struct device_context {
  struct mutex mutex;
  struct device device;
  struct cdev cdev;
  cremona_device_t cremona;
} device_context_t;

struct file_operations *get_file_operatins(void);

void set_device_manager_callbacks(
    cremona_device_manager_callbacks_t *callbacks);
void set_device_callbacks(cremona_device_callbacks_t *collbacks);
void set_toot_callbacks(cremona_toot_callbacks_t *callbacks);

bool create_device_manager_cntext(
    device_manager_context_t *context, unsigned int miner_base,
    unsigned int miner_max, const char *driver_name, const char *class_name,
    int nl_unit);

void destroy_device_manager_context(device_manager_context_t *context);

#endif
