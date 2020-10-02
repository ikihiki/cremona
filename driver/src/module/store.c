#include "store_internal.h"

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

store_t *create_store(uint32_t miner_min, uint32_t miner_max,
                      const char *driver_name, const char *class_name) {
  store_t *store = kzalloc(sizeof(store_t), GFP_KERNEL);

  int alloc_ret =
      alloc_chrdev_region(&store->dev, miner_min, miner_max, driver_name);
  if (alloc_ret != 0) {
    kfree(store);
    printk(KERN_ERR "alloc_chrdev_region = %d\n", alloc_ret);
    return NULL;
  }
  store->device_major = MAJOR(store->dev);
  struct class *device_class = class_create(THIS_MODULE, class_name);
  if (IS_ERR(device_class)) {
    printk(KERN_ERR "class_create\n");
    unregister_chrdev_region(store->dev, miner_max);
    kfree(store);
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

void destroy_store(store_t *store) {

  device_store_t *device;
  uint32_t device_id;
  idr_for_each_entry(&store->devices, device, device_id) {
    remove_device(store, device_id);
  }
  idr_destroy(&store->devices);
  idr_destroy(&store->toots);
  idr_destroy(&store->elements);
  class_destroy(store->device_class);
  unregister_chrdev_region(store->dev, store->miner_max);
  kfree(store);
}

communicator_ref_t get_communicator(store_t *store) {
  return store->communicator;
}

void set_communicator(store_t *store, communicator_ref_t communicator) {
  store->communicator = communicator;
}

void printk_err(crmna_err_t *err) {
  if (err == NULL) {
    return;
  }
  for (int i = 0; i < err->curret_line; err++) {
    printk(KERN_ERR "CREMONA: %s", err->error_msg[i]);
  }
}