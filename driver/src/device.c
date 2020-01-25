#include <linux/bitops.h>
#include <linux/cdev.h>
#include <linux/rwsem.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/uuid.h>

#include "header.h"

static struct toot_data_root list;

struct crmna_device {
  struct device device;
  struct cdev cdev;
  int pid;
  char name[10];
};
void notify_open_toot_result(const struct new_toot_result *result) {
  toot_data_t *data = serch_toot_data(&list, result->uuid);
  data->open_result = result->result;
  data->is_open = 1;
  wake_up_interruptible(&data->open_wait);
}

void notify_add_toot_str_result(const add_toot_str_result_t *result) {
  printk(KERN_ERR "Cremona: %s: reciv wake up write \n", __func__);
  toot_data_t *data = serch_toot_data(&list, result->uuid);
  data->current_writed_result = result->result;
  if (result->result > 0) {
    data->total_writed_byte = data->total_writed_byte + result->result;
    data->current_writed_byte = result->result;
  } else {
    data->current_writed_byte = 0;
  }
  wake_up_interruptible(&data->write_wait);
}

void notify_send_toot_result(const send_toot_result_t *result) {
  toot_data_t *data = serch_toot_data(&list, result->uuid);
  data->close_result = result->result;
  data->is_open = 0;
  wake_up_interruptible(&data->close_wait);
}

static int toot_open(struct inode *inode, struct file *file) {
  printk("myDevice_open\n");
  toot_data_t *data;
  if (file->private_data != NULL) {
    data = file->private_data;
    data->try_open_count++;
  } else {

    struct crmna_device *device =
        container_of(inode->i_cdev, struct crmna_device, cdev);
    if (device == NULL) {
      printk(KERN_ERR "Cremona: %s: Cannot get device data \n", __func__);
    }
    data = kzalloc(sizeof(toot_data_t), GFP_KERNEL);
    init_waitqueue_head(&data->open_wait);
    init_waitqueue_head(&data->write_wait);
    init_waitqueue_head(&data->close_wait);
    generate_random_uuid(data->uuid);
    data->pid = device->pid;
    strncpy(data->name, device->name, 10);
    file->private_data = data;

    struct new_toot *message = kzalloc(sizeof(struct new_toot), GFP_KERNEL);
    for (int i = 0; i < 16; i++) {
      message->uuid[i] = data->uuid[i];
    }
    char payload[50];
    int size = serialize_new_toot(message, payload, 50);
    if (size < 1) {
      printk(KERN_ERR "Cremona: %s: Cannot seriarize new toot in %s\n",
             __func__, device->name);
      kfree(message);
      kfree(data);
      return -1;
    }

    add_toot_data(&list, data);

    int err = send_message(&link, device->pid, CRMNA_NEW_TOOT, payload, size);
    if (err != 0) {
      printk(KERN_ERR "Cremona: %s: Cannot send new toot in %s, because %d\n",
             __func__, device->name, err);
      kfree(message);
      delete_toot_data(&list, data);
      kfree(data);
      return err;
    }
    kfree(message);
  }
  if (data->is_open == 1) {
    if (data->open_result == 0) {
      return 0;
    } else {
      printk(KERN_ERR
             "Cremona: %s: Recive cannot open message in %s, because %d\n",
             __func__, data->name, data->open_result);
      kfree(data);
      return -1;
    }

  } else {
    if (file->f_flags & O_NONBLOCK) {
      if (data->try_open_count < 10) {
        printk(KERN_ERR "Cremona: %s: no wait open\n", __func__);
        return -EAGAIN;
      } else {
        printk(KERN_ERR
               "Cremona: %s: timeout open message in %s, try count %d\n",
               __func__, data->name, data->try_open_count);
        kfree(data);
        return -1;
      }
    } else {
      wait_event_interruptible_timeout(data->open_wait, data->is_open == 1,
                                       10 * HZ / 1000);
    }
  }
  if (data->is_open == 1 && data->open_result == 0) {
    return 0;
  } else {
    printk(KERN_ERR
           "Cremona: %s: cannot recive open message in %s, because %d or %d\n",
           __func__, data->name, data->open_result, data->is_open);
    kfree(data);
    return -1;
  }
}

static ssize_t toot_write(struct file *filp, const char __user *buf,
                          size_t count, loff_t *f_pos) {
  char stored_value[100];
  int cnt = count < 99 ? count : 99;
  printk("myDevice_write\n");
  if (copy_from_user(stored_value, buf, cnt) != 0) {
    return -EFAULT;
  }
  printk("%s\n", stored_value);

  toot_data_t *data;
  data = filp->private_data;
  if (data == NULL) {
    return -EFAULT;
  }
  add_toot_str_t *message = kzalloc(sizeof(add_toot_str_t), GFP_KERNEL);
  if (message == NULL) {
    return -EFAULT;
  }
  for (int i = 0; i < 16; i++) {
    message->uuid[i] = data->uuid[i];
  }
  message->text = stored_value;
  message->text[99] = '\0';

  char payload[200];
  int size = serialize_add_toot_str(message, payload, 50);
  if (size < 1) {
    printk(KERN_ERR "Cremona: %s: Cannot seriarize new toot in %s\n", __func__,
           data->name);
    kfree(message);
    return -1;
  }

  data->current_writed_byte = 0;
  data->current_writed_result = 0;
  printk(KERN_ERR "Cremona: %s: send add  toot str in %s\n",
         __func__, data->name);
  int err = send_message(&link, data->pid, CRMNA_ADD_TOOT_STR, payload, size);
  if (err != 0) {
    printk(KERN_ERR "Cremona: %s: Cannot add  toot str in %s, because %d\n",
           __func__, data->name, err);
    kfree(message);
    return err;
  }
  kfree(message);
  data->try_write_count = 0;

  if (data->current_writed_result != 0) {
    if (data->current_writed_byte > 0) {
      return data->current_writed_byte;
    } else {
      printk(KERN_ERR
             "Cremona: %s: Recive cannot write message in %s, because %d\n",
             __func__, data->name, data->current_writed_result);
      return -1;
    }

  } else {

    wait_event_interruptible_timeout(
        data->write_wait, data->current_writed_result != 0, 10 * HZ / 1000);
  }
  if (data->current_writed_result != 0 && data->current_writed_byte > 0) {
    return data->current_writed_byte;
  } else {
    printk(KERN_ERR
           "Cremona: %s: cannot recive write message in %s, because %d or %d\n",
           __func__, data->name, data->current_writed_result,
           data->current_writed_byte);
    kfree(data);
    return -1;
  }
  return -1;
}

static int toot_close(struct inode *inode, struct file *file) {
  printk("myDevice_close\n");

  toot_data_t *data;
  data = file->private_data;
  if (data == NULL) {
    return -EFAULT;
  }
  send_toot_t *message = kzalloc(sizeof(send_toot_t), GFP_KERNEL);
  if (message == NULL) {
    return -EFAULT;
  }
  for (int i = 0; i < 16; i++) {
    message->uuid[i] = data->uuid[i];
  }

  char payload[50];
  int size = serialize_send_toot(message, payload, 50);
  if (size < 1) {
    printk(KERN_ERR "Cremona: %s: Cannot seriarize send toot in %s\n", __func__,
           data->name);
    kfree(message);
    return -1;
  }
  int err = send_message(&link, data->pid, CRMNA_SEND_TOOT, payload, size);
  if (err != 0) {
    printk(KERN_ERR "Cremona: %s: Cannot send  toot in %s, because %d\n",
           __func__, data->name, err);
    kfree(message);
    return err;
  }
  kfree(message);
  data->try_write_count = 0;

  if (data->is_open == 0) {
    if (data->close_result == 0) {
      return 0;
    } else {
      printk(KERN_ERR
             "Cremona: %s: Recive cannot close message in %s, because %d\n",
             __func__, data->name, data->close_result);
      return -1;
    }

  } else {
    wait_event_interruptible_timeout(data->close_wait, data->is_open == 0,
                                     10 * HZ / 1000);
  }
  if (data->is_open == 0 && data->close_result == 0) {
    return 0;
  } else {
    printk(KERN_ERR
           "Cremona: %s: cannot recive close message in %s, because %d or %d\n",
           __func__, data->name, data->close_result, data->is_open);
    return -1;
  }
  delete_toot_data(&list, data);
  kfree(data);
  file->private_data = NULL;

  return 0;
}

static struct file_operations ops = {
    .open = &toot_open,
    .release = toot_close,
    .write = &toot_write,
};

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

int create_connection(const char *name, int pid) {
  int miner = -1;

  spin_lock(&miner_list_lock);
  miner = ffs(~miner_list);
  if (miner != 0) {
    miner_list = miner_list | (1 << (miner - 1));
  }
  spin_unlock(&miner_list_lock);
  if (miner == 0) {
    printk(KERN_ERR "Cremona: %s: Cannot register %s no space\n", __func__,
           name);
    return CRMNA_CREATE_CONN_ERR_NO_SPACE;
  }

  struct crmna_device *dev = kzalloc(sizeof(struct crmna_device), GFP_KERNEL);
  if (dev == NULL) {
    printk(KERN_ERR "Cremona: %s: Fail allocate device %s", __func__, name);
    release_flag(miner);
    return CRMNA_CREATE_CONN_ERR_FAIL_CREATE_DEVICE;
  }

  device_initialize(&dev->device);
  dev->device.class = mydevice_class;
  dev->device.devt = MKDEV(mydevice_major, miner);
  kobject_set_name(&(dev->device.kobj), "crmna_%s", name);

  cdev_init(&dev->cdev, &ops);
  dev->cdev.owner = THIS_MODULE;

  strncpy(dev->name, name, 10);
  dev->pid = pid;

  int err = cdev_device_add(&dev->cdev, &dev->device);
  if (err != 0) {
    printk(KERN_ERR "Cremona: %s: Fail add device %s, because %d", __func__,
           name, err);
    kfree(dev);
    return CRMNA_CREATE_CONN_ERR_FAIL_CREATE_DEVICE;
  }

  if (check_flag(miner) == 0) {
    printk(KERN_ERR "Cremona: %s: Cannot register %s as miner: %d\n", __func__,
           name, miner);
    cdev_device_del(&dev->cdev, &dev->device);
    kfree(dev);
    return CRMNA_CREATE_CONN_ERR_FAIL_ALLOC;
  }

  return miner;
}

void destroy_connection(int miner) {
  struct device *device;
  device =
      class_find_device_by_devt(mydevice_class, MKDEV(mydevice_major, miner));
  struct crmna_device *dev = container_of(device, struct crmna_device, device);
  if (dev) {
    cdev_device_del(&dev->cdev, &dev->device);
    kfree(dev);
  }
  release_flag(miner);
}

void destroy_all_connections(void) {
  int i;
  for (i = 1; i <= 64; i++) {
    spin_lock(&miner_list_lock);
    int flag = miner_list & (1 << (i - 1));
    spin_unlock(&miner_list_lock);

    if (flag != 0) {
      destroy_connection(i);
    }
  }
}

void init_devices(void) { init_toot_data_root(&list); }