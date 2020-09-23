#include "module.h"
#include "store_internal.h"

bool set_toot_id(unsigned int toot_id, void *set_toot_id_context,
                 crmna_err_t *err) {
  unsigned int *data = kmalloc(sizeof(unsigned int), GFP_KERNEL);
  if (data == NULL) {
    ADD_ERROR(err, "cannot allocate data");
    return false;
  }
  *data = toot_id;
  ((struct file *)set_toot_id_context)->private_data = data;
  return true;
}

static int toot_open(struct inode *inode, struct file *file) {
  DEFINE_ERROR(err);

  device_store_t *device = container_of(inode->i_cdev, device_store_t, cdev);

  action_t action =
      create_action_create_toot(device->device_id, &set_toot_id, file);
  if (!dispatch(device->store, &action, &err)) {
    printk_err(&err);
    return -EFAULT;
  }

  return 0;
}

static int toot_close(struct inode *inode, struct file *file) {
  DEFINE_ERROR(err);
  if (file->private_data == NULL) {
    return -EFAULT;
  }
  unsigned int toot_id = *(unsigned int *)file->private_data;
  device_store_t *device = container_of(inode->i_cdev, device_store_t, cdev);
  action_t action = create_action_send_toot(toot_id);
  if (!dispatch(device->store, &action, &err)) {
    kfree(file->private_data);
    printk_err(&err);
    return -EFAULT;
  }
  kfree(file->private_data);
  return 0;
}

static ssize_t toot_write(struct file *file, const char __user *buf,
                          size_t count, loff_t *f_pos) {
  DEFINE_ERROR(err);
  if (file->private_data == NULL) {
    return -EFAULT;
  }
  unsigned int toot_id = *(unsigned int *)file->private_data;
  device_store_t *device =
      container_of(file->f_inode->i_cdev, device_store_t, cdev);

  DEFINE_CRMNA_BUF(stored_value, 100);
  int cnt = count < 99 ? count : 99;
  printk("myDevice_write\n");
  if (copy_from_user(stored_value_buffer_content, buf, cnt) != 0) {
    return -EFAULT;
  }
  printk("%s\n", stored_value_buffer_content);
  action_t action = create_action_add_toot_element(toot_id, &stored_value);
  if (!dispatch(device->store, &action, &err)) {
    printk_err(&err);
    return -EFAULT;
  }
  return cnt;
}

static struct file_operations ops = {
    .open = &toot_open,
    .release = toot_close,
    .write = &toot_write,
};

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
  device->store = store;
  idr_replace(&store->devices, device, device_id);
  *id = device_id;

  return true;
}

void remove_device(store_t *store, unsigned int device_id) {
  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, device_id);
  if (device == NULL) {
    return;
  }
  down_write(&device->semaphore);
  device->is_ready = false;
  up_write(&device->semaphore);

  toot_store_t *toot;
  int toot_id;
  idr_for_each_entry(&store->toots, toot, toot_id) {
    if (toot->device_id == device_id) {
      remove_toot(store, toot_id);
    }
  }

  spin_lock(&store->devices_lock);
  idr_remove(&store->devices, device_id);
  spin_unlock(&store->devices_lock);

  kfree(device);
}

bool attach_device_class(store_t *store, unsigned int device_id,
                         crmna_err_t *err) {
  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, device_id);
  if (device == NULL) {
    ADD_ERROR(err, "canot find device id");
    return false;
  }

  down_write(&device->semaphore);
  device_initialize(&device->device);
  device->device.class = store->device_class;
  device->device.devt = device->dev;
  kobject_set_name(&(device->device.kobj), "crmna_%s", device->name);
  cdev_init(&device->cdev, &ops);
  device->cdev.owner = THIS_MODULE;
  up_write(&device->semaphore);
  return true;
}

void detach_device_class(store_t *store, unsigned int device_id) {
  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, device_id);
  if (device == NULL) {
    return;
  }
  down_write(&device->semaphore);
  cdev_device_del(&device->cdev, &device->device);
  up_write(&device->semaphore);
}

void set_device_ready(store_t *store, unsigned int device_id) {
  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, device_id);
  if (device == NULL) {
    return;
  }
  down_write(&device->semaphore);
  device->is_ready = true;
  up_write(&device->semaphore);
}

bool check_device_ready(store_t *store, unsigned int device_id,
                        crmna_err_t *err) {
  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, device_id);
  if (device == NULL) {
    ADD_ERROR(err, "canot find device_id");
    return false;
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
