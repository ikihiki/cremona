#include "header.h"
#ifdef __KERNEL__

static void crmna_toot_lock(cremona_toot_t *toot) {
  toot_context_t *context = container_of(toot, toot_context_t, cremona);
  mutex_lock(&context->mutex);
}
static void crmna_toot_unlock(cremona_toot_t *toot) {
  toot_context_t *context = container_of(toot, toot_context_t, cremona);
  mutex_unlock(&context->mutex);
}
static void crmna_toot_wait(cremona_toot_t *toot, toot_wait_type_t wait_type) {
  toot_context_t *context = container_of(toot, toot_context_t, cremona);
  printk(KERN_INFO "Cremona: %s: wait type: %d toot id: %llu\n", __func__,
         wait_type, toot->id);

  switch (wait_type) {
  case WAIT_OPEN:
    wait_event_interruptible_timeout(context->wait_head,
                                     context->cremona.state == OPEND ||
                                         context->cremona.state == DESTROYED ||
                                         context->cremona.state == ERROR,
                                     10 * HZ / 1000);
    break;
  case WAIT_CLOSE:
    wait_event_interruptible_timeout(context->wait_head,
                                     context->cremona.state == DESTROYED ||
                                         context->cremona.state == ERROR,
                                     10 * HZ / 1000);
  case WAIT_WRITE:
    wait_event_interruptible_timeout(context->wait_head,
                                     context->cremona.state ==
                                             WRITE_COMPLEATE ||
                                         context->cremona.state == DESTROYED ||
                                         context->cremona.state == ERROR,
                                     10 * HZ / 1000);
    break;
  default:
    break;
  }
  printk(KERN_INFO "Cremona: %s: wake type: %d toot id: %llu\n", __func__,
         wait_type, toot->id);
}
static void crmna_toot_notify(cremona_toot_t *toot) {
  printk(KERN_INFO "Cremona: %s: notify state: %d toot id: %llu\n", __func__,
         toot->state, toot->id);
  toot_context_t *context = container_of(toot, toot_context_t, cremona);
  wake_up_interruptible(&context->wait_head);
}
static cremona_toot_t *crmna_toot_create_toot(void) {
  toot_context_t *context = kmalloc(sizeof(toot_context_t), GFP_KERNEL);
  if (context == NULL) {
    return NULL;
  }
  mutex_init(&context->mutex);
  init_waitqueue_head(&context->wait_head);

  return &context->cremona;

err:
  kfree(context);
  return NULL;
}
static bool crmna_toot_destroy_toot(cremona_toot_t *toot) { return true; }
static void crmna_toot_cleanup_toot(cremona_toot_t *toot) {
  toot_context_t *context = container_of(toot, toot_context_t, cremona);

  mutex_destroy(&context->mutex);
  kfree(context);
}

static int toot_open(struct inode *inode, struct file *file) {
  // printk(KERN_INFO "Cremona: %s: toot open\n", __func__);

  // toot_context_t *toot_context;

  // if (file->private_data != NULL) {
  //   toot_context = (toot_context_t *)file->private_data;
  // } else {

  //   char error_msg[100];
  //   crmna_err err = {.error_msg = error_msg,
  //                      .error_msg_len = sizeof(error_msg)};

  //   bool wait = (file->f_flags & O_NONBLOCK) == 0;
  //   device_context_t *device_context =
  //       container_of(inode->i_cdev, device_context_t, cdev);
  //   cremona_toot_t *toot = open_toot(&device_context->cremona, wait, &err);
  //   toot_context = container_of(toot, toot_context_t, cremona);
  //   file->private_data = (void *)toot_context;
  // }
  // mutex_lock(&toot_context->mutex);
  // int result = -EIO;
  // if (toot_context->cremona.state == OPEND) {
  //   result = 0;
  // }
  // if (toot_context->cremona.state == OPEN_RESULT_WAIT) {
  //   result = -EAGAIN;
  // }
  // mutex_unlock(&toot_context->mutex);
  // printk(KERN_INFO "Cremona: %s: toot open compleate result: %d\n", __func__,
  //        result);

  // return result;
  return 0;
}

static int toot_close(struct inode *inode, struct file *file) {
  // printk(KERN_INFO "Cremona: %s: toot close\n", __func__);

  // if (file->private_data == NULL) {
  //   return 0;
  // }
  // toot_context_t *toot_context = (toot_context_t *)file->private_data;

  // if (toot_context->cremona.state == DESTROYED ||
  //     toot_context->cremona.state == TOOT_ERROR) {
  //   file->private_data = NULL;
  //   release_toot(&toot_context->cremona);
  //   return 0;
  // }

  // char error_msg[100];
  // crmna_err err = {.error_msg = error_msg,
  //                    .error_msg_len = sizeof(error_msg)};

  // bool result = close_toot(&toot_context->cremona, &err);
  // if (result) {
  //   file->private_data = NULL;
  //   release_toot(&toot_context->cremona);
  //   return 0;
  // } else {
  //   file->private_data = NULL;
  //   release_toot(&toot_context->cremona);
  //   return -EIO;
  // }
}

static ssize_t toot_write(struct file *file, const char __user *buf,
                          size_t count, loff_t *f_pos) {
  // printk(KERN_INFO "Cremona: %s: add toot text\n", __func__);

  // if (file->private_data == NULL) {
  //   return -EFAULT;
  // }
  // char stored_value[100];
  // int cnt = count < 99 ? count : 99;
  // printk("myDevice_write\n");
  // if (copy_from_user(stored_value, buf, cnt) != 0) {
  //   return -EFAULT;
  // }
  // printk("%s\n", stored_value);

  // toot_context_t *toot_context = (toot_context_t *)file->private_data;

  // char error_msg[100];
  // crmna_err err = {.error_msg = error_msg,
  //                    .error_msg_len = sizeof(error_msg)};
  // bool wait = true; //(file->f_flags & O_NONBLOCK) == 0;
  // if (!add_toot_text(&toot_context->cremona, stored_value, wait, &err)) {
  //   return -EFAULT;
  // }
  // int result;
  // mutex_lock(&toot_context->mutex);
  // if (toot_context->cremona.state == ADD_TEXT_RESULT_WAIT) {
  //   result = -EAGAIN;
  // } else if (toot_context->cremona.state == WRITE_COMPLEATE) {
  //   toot_context->cremona.state = OPEND;
  //   result = toot_context->cremona.prev_count;
  // } else {
  //   result = -EFAULT;
  // }
  // mutex_unlock(&toot_context->mutex);
  // printk(KERN_INFO "Cremona: %s: add toot compleate\n", __func__);
  // return result;
}

static struct file_operations ops = {
    .open = &toot_open,
    .release = toot_close,
    .write = &toot_write,
};

struct file_operations *get_file_operatins(void) {
  return &ops;
}

void set_toot_callbacks(cremona_toot_callbacks_t *callbacks) {
  callbacks->create_toot = &crmna_toot_create_toot;
  callbacks->wait = &crmna_toot_wait;
  callbacks->notify = &crmna_toot_notify;
  callbacks->lock = &crmna_toot_lock;
  callbacks->unlock = &crmna_toot_unlock;
  callbacks->destroy_toot = &crmna_toot_destroy_toot;
  callbacks->cleanup_toot = &crmna_toot_cleanup_toot;
}
#endif