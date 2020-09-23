#ifndef __CREMONA__STORE_INTERNAL__
#define __CREMONA__STORE_INTERNAL__

#include "../cremona/central_store.h"
#include <linux / spinlock.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/idr.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/uaccess.h>

typedef struct store {
  struct idr devices;
  struct idr toots;
  struct idr elements;
  unsigned int miner_min;
  unsigned int miner_max;
  dev_t dev;
  struct class *device_class;
  unsigned int device_major;
  spinlock_t devices_lock;
  spinlock_t toots_lock;
  spinlock_t elements_lock;
  communicator_ref_t communicator;
} store_t;

typedef struct {
  unsigned int device_id;
  int pid;
  int uid;
  char *name;
  struct cdev cdev;
  dev_t dev;
  struct device device;
  struct rw_semaphore semaphore;
  bool is_ready;
  store_t *store;
} device_store_t;

bool check_device_ready(store_t *store, unsigned int device_id,
                               crmna_err_t *err);

typedef enum toot_state
{
    CREANUPED,
    OPEN_RESULT_WAIT,
    OPEND,
    CLOSE_RESULT_WAIT,
    DESTROYED,
    TOOT_ERROR
} toot_state_t;

typedef struct {
  unsigned int toot_id;
  unsigned int device_id;
  wait_queue_head_t wait_head;
  spinlock_t spinlock;
  toot_state_t state;
} toot_store_t;

bool check_toot_ready(store_t *store, unsigned int toot_id,
                      crmna_err_t *err);

toot_store_t *get_toot_when_ready(store_t *store, unsigned int toot_id,
                                  crmna_err_t *err);

typedef enum element_state
{
    ELEMENT_CREANUPED,
    ELEMENT_READY,
    ELEMENT_SEND_WAIT,
    ELEMENT_SENT,
    ELEMENT_ERROR
} element_state_t;

typedef struct {
  unsigned int element_id;
  unsigned int toot_id;
  wait_queue_head_t wait_head;
  spinlock_t spinlock;
  element_state_t state;
} element_store_t;
element_store_t *get_element_when_ready(store_t *store, unsigned int element_id,
                                        crmna_err_t *err);
#endif