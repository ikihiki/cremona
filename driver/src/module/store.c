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
} device_store_t;

static int toot_open(struct inode *inode, struct file *file) {}

static int toot_close(struct inode *inode, struct file *file) {}

static ssize_t toot_write(struct file *file, const char __user *buf,
                          size_t count, loff_t *f_pos) {}

static struct file_operations ops = {
    .open = &toot_open,
    .release = toot_close,
    .write = &toot_write,
};

static void get_ownership(struct device *dev, kuid_t *uid, kgid_t *gid) {
  device_store_t *device_store = container_of(dev, device_store_t, device);
  uid->val = device_store->uid;
  printk("Cremona: get ownership. uid %u\n", device_store->uid);
}

static int driver_uevent(struct device *dev, struct kobj_uevent_env *env) {
  device_store_t *device_store = container_of(dev, device_store_t, device);
  add_uevent_var(env, "DEVUID=%u", device_store->uid);
  return 0;
}

store_t *create_store(unsigned int miner_min, unsigned int miner_max,
                      const char *driver_name, const char *class_name) {
  store_t *store = kzalloc(sizeof(store_t), GFP_KERNEL);

  int alloc_ret =
      alloc_chrdev_region(&store->dev, miner_min, miner_max, driver_name);
  if (alloc_ret != 0) {
    free(store);
    printk(KERN_ERR "alloc_chrdev_region = %d\n", alloc_ret);
    return NULL;
  }
  store->device_major = MAJOR(store->dev);
  struct class *device_class = class_create(THIS_MODULE, class_name);
  if (IS_ERR(device_class)) {
    printk(KERN_ERR "class_create\n");
    unregister_chrdev_region(dev, miner_max);
    free(store);
    return false;
  }
  store->device_class = device_class;
  store->device_class->get_ownership = &get_ownership;
  store->device_class->dev_uevent = &driver_uevent;

  idr_init(&store->devices);
  idr_init(&store->toots);
  idr_init(&store->elements);
  spin_lock_init(&store->devices_lock);
  spin_lock_init(&store->toots_lock);
  spin_lock_init(&store->elements_lock);
  store->miner_max = miner_max;
  store->miner_min = miner_min;
  clear_communicator_ref(&store->communicator);

  return store;
}

void destroy_store(store_t *store) {}

// start device
bool add_device(store_t *store, int pid, int uid, char *name, int *id,
                crmna_err_t *err) {
  char *memorize_name = kmalloc(sizeof(char) * strlen(name), GFP_KERNEL);
  if (memorize_name == NULL) {
    ADD_ERROR(err, "fail name allocation");
    return false;
  }
  strcpy(memorize_name, name);

  idr_preload(GFP_KERNEL);
  spin_lock(&store->devices_lock);
  int allocate_result = idr_alloc(&store->devices, NULL, store->miner_min,
                                  store->miner_max, GFP_KERNEL);
  spin_unlock(&store->devices_lock);
  idr_preload_end();
  if (allocate_result == -ENOMEM) {
    kfree(memorize_name);
    return false;
  }
  if (allocate_result == -ENOSPC) {
    kfree(memorize_name);
    return false;
  }

  unsigned int device_id = (unsigned int)allocate_result;

  device_store_t *device = kmalloc(sizeof(device_store_t), GFP_KERNEL);
  if (device == NULL) {
    kfree(memorize_name);
    spin_lock(&store->devices_lock);
    idr_remove(&store->devices, device_id);
    spin_unlock(&store->devices_lock);
    return false;
  }
  device->device_id = device_id;
  device->pid = pid;
  device->uid = uid;
  device->name = memorize_name;
  device->dev = MKDEV(store->device_major, device_id);
  init_rwsem(&device->semaphore);
  device->is_ready = false;
  idr_replace(&store->devices, device, device_id);
  *id = device_id;

  return true;
}

bool attach_device_class(store_t *store, unsigned int device_id,
                         crmna_err_t *err) {
  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, device_id);
  if (device == NULL) {
    ADD_ERROR(err, "canot find id");
    return false;
  }

  down_write(&device->semaphore);
  device_initialize(&device->device);
  device->device.class = store->device_class;
  device->device.devt = device->dev;
  kobject_set_name(&(device->device.kobj), "crmna_%s", device->name);
  cdev_init(&device->cdev, *ops);
  device->cdev.owner = THIS_MODULE;
  up_write(&device->semaphore);
  return true;
}

void detach_device_class(store_t *store, unsigned int device_id) {
  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, device_id);
  if (device == NULL) {
    ADD_ERROR(err, "canot find id");
    return false;
  }
  down_write(&device->semaphore);
  cdev_device_del(&device->cdev, &device->device);
  up_write(&device->semaphore);
}

void set_device_ready(store_t *store, unsigned int device_id) {
  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, device_id);
  if (device == NULL) {
    ADD_ERROR(err, "canot find id");
    return false;
  }
  down_write(&device->semaphore);
  device->is_ready = true;
  up_write(&device->semaphore);
}

static bool check_device_ready(store_t *store, unsigned int device_id,
                               crmna_err_t *err) {
  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, device_id);
  if (device == NULL) {
    ADD_ERROR(err, "canot find device_id");
    result = false;
  }

  bool result = true;

  down_read(&device->semaphore);
  if (!device->is_ready) {
    ADD_ERROR(err, "device is not ready");
    result = false;
  }
  up_read(&device->semaphore);

  return result;
}
// end device

communicator_ref_t get_communicator(store_t *store) {
  return store->communicator;
}

void set_communicator(store_t *store, communicator_ref_t communicator) {
  store->communicator = communicator;
}

// start toot
typedef enum toot_state {
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

bool add_toot(store_t *store, unsigned int device_id, unsigned int *toot_id,
              crmna_err_t *err) {
  if (!check_device_ready(store, device_id, err)) {
    return false;
  }

  idr_preload(GFP_KERNEL);
  spin_lock(&store->toots_lock);
  int allocate_result = idr_alloc(&store->toots, NULL, 0, INT_MAX, GFP_KERNEL);
  spin_unlock(&store->toots_lock);
  idr_preload_end();
  if (allocate_result == -ENOMEM) {
    ADD_ERROR(err, "canot allocate toot. because no memory");
    return false;
  }
  if (allocate_result == -ENOSPC) {
    ADD_ERROR(err, "canot allocate toot. because no id space");
    return false;
  }

  unsigned int id = (unsigned int)allocate_result;
  toot_store_t *toot = kmalloc(sizeof(toot_store_t), GFP_KERNEL);
  if (toot = NULL) {
    spin_lock(&store->toots_lock);
    idr_remove(&store->toots, id);
    spin_unlock(&store->toots_lock);
    ADD_ERROR(err, "canot allocate toot. fail malloc");
    return false;
  }

  toot->device_id = device_id;
  toot->toot_id = id;
  toot->state = OPEN_RESULT_WAIT;
  init_waitqueue_head(&toot->wait_head);
  spin_lock_init(&toot->spinlock);
  idr_replace(&store->toots, toot, id);
  *toot_id = id;

  return true;
}
void remove_toot(store_t *store, unsigned int toot_id) {}
bool wait_toot_ready_or_failer(store_t *store, unsigned int toot_id,
                               crmna_err_t *err) {
  toot_store_t *toot = (toot_store_t *)idr_find(&store->toots, toot_id);
  if (toot == NULL) {
    ADD_ERROR(err, "canot find id.");
    return false;
  }
  wait_event_interruptible_timeout(
      toot->wait_head,
      toot->state == OPEND || toot->state == DESTROYED || toot->state == ERROR,
      10 * HZ / 1000);

  return true;
}
bool wait_toot_sent_or_failer(store_t *store, unsigned int toot_id,
                              crmna_err_t *err) {
  toot_store_t *toot = (toot_store_t *)idr_find(&store->toots, toot_id);
  if (toot == NULL) {
    ADD_ERROR(err, "canot find id.");
    return false;
  }
  wait_event_interruptible_timeout(
      toot->wait_head, toot->state == DESTROYED || toot->state == ERROR,
      10 * HZ / 1000);

  return true;
}
void set_toot_failer(store_t *store, unsigned int toot_id) {
  toot_store_t *toot = (toot_store_t *)idr_find(&store->toots, toot_id);
  if (toot == NULL) {
    return;
  }
  spin_lock(&toot->spinlock);
  toot->state = TOOT_ERROR;
  spin_unlock(&toot->spinlock);

  wake_up_interruptible(&toot->wait_head);
}
void set_toot_ready(store_t *store, unsigned int toot_id) {
  toot_store_t *toot = (toot_store_t *)idr_find(&store->toots, toot_id);
  if (toot == NULL) {
    return;
  }
  spin_lock(&toot->spinlock);
  toot->state = OPEND;
  spin_unlock(&toot->spinlock);

  wake_up_interruptible(&toot->wait_head);
}
void set_toot_sent(store_t *store, unsigned int toot_id) {
  toot_store_t *toot = (toot_store_t *)idr_find(&store->toots, toot_id);
  if (toot == NULL) {
    return;
  }
  spin_lock(&toot->spinlock);
  toot->state = DESTROYED;
  spin_unlock(&toot->spinlock);

  wake_up_interruptible(&toot->wait_head);
}
bool get_device_id_from_toot(store_t *store, unsigned int toot_id,
                             unsigned int *device_id) {
  toot_store_t *toot = (toot_store_t *)idr_find(&store->toots, toot_id);
  if (toot == NULL) {
    return false;
  }
  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, toot->device_id);
  if (device == NULL) {
    return false;
  }
  *device_id = toot->device_id;
  return true;
}
bool get_device_pid_from_toot(store_t *store, unsigned int toot_id, int *pid) {
  toot_store_t *toot = (toot_store_t *)idr_find(&store->toots, toot_id);
  if (toot == NULL) {
    return false;
  }
  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, toot->device_id);
  if (device == NULL) {
    return false;
  }
  *pid = device->pid;
  return true;
}

static bool check_toot_ready(store_t *store, unsigned int toot_id,
                             crmna_err_t *err) {
  toot_store_t *toot = (toot_store_t *)idr_find(&store->toots, toot_id);
  if (toot == NULL) {
    ADD_ERROR(err, "canot find id.");
    return false;
  }

  spin_lock(&toot->spinlock);
  if (!toot->state == OPEND) {
    spin_unlock(&toot->spinlock);
    ADD_ERROR(err, "toot is not ready");
    return false;
  }
  unsigned int device_id = toot->device_id;
  spin_unlock(&toot->spinlock);

  return check_device_ready(store, device_id, err);
}

// end toot

// start element

typedef enum element_state {
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
bool add_element(store_t *store, unsigned int toot_id, unsigned int *element_id,
                 crmna_err_t *err) {
  if (!check_toot_ready(store, toot_id, err)) {
    return false;
  }

  idr_preload(GFP_KERNEL);
  spin_lock(&store->elements_lock);
  int allocate_result =
      idr_alloc(&store->elements, NULL, 0, INT_MAX, GFP_KERNEL);
  spin_unlock(&store->elements_lock);
  idr_preload_end();
  if (allocate_result == -ENOMEM) {
    ADD_ERROR(err, "canot allocate toot. because no memory");
    return false;
  }
  if (allocate_result == -ENOSPC) {
    ADD_ERROR(err, "canot allocate toot. because no id space");
    return false;
  }

  unsigned int id = (unsigned int)allocate_result;
  element_store_t *element = kmalloc(sizeof(element_store_t), GFP_KERNEL);
  if (toot = NULL) {
    spin_lock(&store->elements_lock);
    idr_remove(&store->elements, id);
    spin_unlock(&store->elements_lock);
    ADD_ERROR(err, "canot allocate toot. fail malloc");
    return false;
  }

  element->element_id = id;
  element->toot_id = toot_id;
  element->state = ELEMENT_READY;
  init_waitqueue_head(&element->wait_head);
  spin_lock_init(&element->spinlock);
  *element_id = id;
  return true;
}
void remove_element(store_t *store, unsigned int element_id) {}
bool wait_element_sent_or_failer(store_t *store, unsigned int element_id,
                                 crmna_err_t *err) {
  element_store_t *element =
      (element_store_t *)idr_find(&store->elements, element_id);
  if (element == NULL) {
    ADD_ERROR(err, "canot find id.");
    return false;
  }
  wait_event_interruptible_timeout(element->wait_head,
                                   element->state == ELEMENT_SENT ||
                                       element->state == ELEMENT_ERROR,
                                   10 * HZ / 1000);
}
void set_element_sent(store_t *store, unsigned int element_id) {
  element_store_t *element =
      (element_store_t *)idr_find(&store->elements, element_id);
  if (element == NULL) {
    return false;
  }
  spin_lock(&element->spinlock);
  toot->state = ELEMENT_SENT;
  spin_unlock(&element->spinlock);

  wake_up_interruptible(&element->wait_head);
}
void set_element_failer(store_t *store, unsigned int element_id) {
  element_store_t *element =
      (element_store_t *)idr_find(&store->elements, element_id);
  if (element == NULL) {
    return false;
  }
  spin_lock(&element->spinlock);
  toot->state = ELEMENT_ERROR;
  spin_unlock(&element->spinlock);

  wake_up_interruptible(&element->wait_head);
}
