// #include "header.h"

// static void crmna_device_lock(cremona_device_t *device) {
//   device_context_t *context = container_of(device, device_context_t, cremona);
//   mutex_lock(&(context->mutex));
// }
// static void cremona_device_unlock(cremona_device_t *device) {
//   device_context_t *context = container_of(device, device_context_t, cremona);
//   mutex_unlock(&(context->mutex));
// }

// static void get_ownership(struct device *dev, kuid_t *uid, kgid_t *gid) {
//   device_context_t *device_context =
//       container_of(dev, device_context_t, device);
//   uid->val = device_context->cremona.uid;
//   printk("Cremona: get ownership. uid %u\n", device_context->cremona.uid);
// }

// static int testdriver_uevent(struct device *dev, struct kobj_uevent_env *env) {
//   device_context_t *device_context =
//       container_of(dev, device_context_t, device);
//   add_uevent_var(env, "DEVUID=%u", device_context->cremona.uid);

//   return 0;
// }

// cremona_device_t *crmna_init_device(cremona_device_manager_t *device_manager,
//                                     char *name, unsigned int miner) {

//   device_manager_context_t *manager_context =
//       container_of(device_manager, device_manager_context_t, cremona);

//   device_context_t *context = kmalloc(sizeof(device_context_t), GFP_KERNEL);
//   if (context == NULL) {
//     return NULL;
//   }

//   mutex_init(&(context->mutex));
//   device_initialize(&(context->device));
//   context->device.class = manager_context->device_class;
//   context->device.devt = MKDEV(manager_context->major_num, miner);
//   context->device.class->get_ownership = &get_ownership;
//   context->device.class->dev_uevent = &testdriver_uevent;
//   kobject_set_name(&(context->device.kobj), "crmna_%s", name);

//   cdev_init(&context->cdev, get_file_operatins());
//   context->cdev.owner = THIS_MODULE;
//   return &context->cremona;
// }

// static bool crmna_create_device(cremona_device_t *device) {
//   device_context_t *context = container_of(device, device_context_t, cremona);
//   int err = cdev_device_add(&context->cdev, &context->device);
//   if (err != 0) {
//     // LOG_ERROR(device->device_manager, "Fail add device %s, because %d",
//     //           device->name, err);
//     return false;
//   }
//   return true;
// }

// bool crmna_destroy_device(cremona_device_t *device) {
//   device_context_t *context = container_of(device, device_context_t, cremona);
//   cdev_device_del(&context->cdev, &context->device);
//   return true;
// }
// void crmna_cleanup_device(cremona_device_t *device) {
//   device_context_t *context = container_of(device, device_context_t, cremona);
//   mutex_destroy(&(context->mutex));
//   kfree(context);
// }

// void set_device_callbacks(cremona_device_callbacks_t *collbacks) {
//   collbacks->lock = &crmna_device_lock;
//   collbacks->unlock = &cremona_device_unlock;
//   collbacks->init_device = &crmna_init_device;
//   collbacks->create_device = &crmna_create_device;
//   collbacks->destroy_device = &crmna_destroy_device;
//   collbacks->cleanup_device = &crmna_cleanup_device;
// }
