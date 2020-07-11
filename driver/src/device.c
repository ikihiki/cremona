#include "cremona_internal.h"
#include "message.h"

cremona_device_t *add_ref_device(cremona_device_t *device) {
  if (device == NULL) {
    return NULL;
  }
  locker_lock(&device->lock);
  if (device->isDestroied) {
    locker_unlock(&device->lock);
    LOG_ERROR(device->logger_ref,
              "Device already destroyed. device id: %llu, device name: %s",
              device->miner, device->name);
    return NULL;
  }

  device->refCount++;
  locker_unlock(&device->lock);
  return device;
}

cremona_device_t *
create_device(int miner, uint32_t pid, uint32_t uid, char *name,
              id_mapper_factory_ref *id_mapper_factory,
              locker_factory_ref *locker_factory, communicate_t *comm,
              waiter_factory_ref *waiter_factory,
              device_file_factory_ref *device_file_factory, logger *logger_ref,
              allocator_ref *alloc, crmna_err *error) {

  cremona_device_t *device =
      (cremona_device_t *)allocator_allocate(alloc, sizeof(cremona_device_t));
  if (device == NULL) {
    LOG_AND_ADD_ERROR(logger_ref, error, "Cannot allocate device. pid: %d",
                        pid);
    return NULL;
  }

  device->isDestroied = false;
  device->refCount = 0;
  device->miner = miner;
  device->pid = pid;
  device->uid = uid;
  device->logger_ref = logger_ref;
  device->comm = comm;
  device->id_mapper_factory = id_mapper_factory;
  device->locker_factory = locker_factory;
  device->waiter_factory = waiter_factory;
  device->alloc = alloc;
  memcpy(device->name, name, sizeof(device->name));
  clear_id_mapper_ref(&device->toots);
  clear_locker_ref(&device->lock);
  clear_device_file_ref(&device->device_file);

  if (!create_locker(locker_factory, &device->lock, error)) {
    LOG_AND_ADD_ERROR(logger_ref, error,
                        "Cannot allocate locker in device. pid: %d", pid);
    goto err;
  }

  if (!create_id_mapper(id_mapper_factory, &device->toots, error)) {
    LOG_AND_ADD_ERROR(logger_ref, error,
                        "Cannot allocate id_mapper in device. pid: %d", pid);
    goto err;
  }

  if (!create_device_file(device_file_factory, device, &device->device_file,
                          error)) {
    LOG_AND_ADD_ERROR(logger_ref, error,
                        "Cannot create device file in device. pid: %d", pid);
    goto err;
  }

  LOG_INFO(device->logger_ref,
           "Device created. name: %s id: %ld pid: %u uid: %u", device->name,
           device->miner, device->pid, device->uid);

  device->refCount++;
  return device;

err:
  if (device->device_file.interface != NULL) {
    device_file_free(&device->device_file);
  }
  if (device->toots.interface != NULL) {
    id_mapper_free(&device->toots);
  }
  if (device->lock.interface != NULL) {
    locker_free(&device->lock);
  }
  allocator_free(alloc, device);
  return NULL;
}

void destroy_device(cremona_device_t *device) {
  locker_lock(&device->lock);
  device->isDestroied = true;
  locker_unlock(&device->lock);
  LOG_INFO(device->logger_ref, "Device destroyed. name: %s id: %ld pid: %u",
           device->name, device->miner, device->pid);
}

void release_device(cremona_device_t *device) {
  if (device == NULL) {
    return;
  }
  locker_lock(&device->lock);
  device->refCount--;
  if (device->refCount > 0) {
    locker_unlock(&device->lock);
    return;
  }

  if (device->refCount < 0) {
    LOG_WARN(device->logger_ref,
             "refCount is under 0 {\"id\": %ld, \"pid\": %d, \"refCount\": %d}",
             device->miner, device->pid, device->refCount);
  }
  if (device->isDestroied == false) {
    LOG_WARN(device->logger_ref,
             "not destroied but refCount is less or equal 0. {\"id\": %llu, "
             "\"pid\": %d, \"refCount\": %d}",
             device->miner, device->pid, device->refCount);
  }
  // if (count_toot(device) > 0) {
  //   LOG_WARN(device->logger_ref,
  //            "toot map is not empty but refCount is less or equal 0. "
  //            "{\"id\": %llu, "
  //            "\"pid\": %d, \"refCount\": %d}",
  //            device->miner, device->pid, device->refCount);
  // }

  locker_unlock(&device->lock);
  device_file_free(&device->device_file);
  id_mapper_free(&device->toots);
  locker_free(&device->lock);
  allocator_free(device->alloc, device);

  LOG_INFO(device->logger_ref, "Device deallocated. name: %s id: %llu",
           device->name, device->miner);
}

cremona_toot_t *open_toot(cremona_device_t *device, bool wait,
                          crmna_err *error) {
  locker_lock(&device->lock);
  if (device->isDestroied) {
    locker_unlock(&device->lock);
    LOG_AND_ADD_ERROR(
        device->logger_ref, error,
        "Device already destroyed. device id: %llu, device name: %s",
        device->miner, device->name);
    return NULL;
  }
  locker_unlock(&device->lock);

  int id = -1;
  if (!id_mapper_add_get_id(&device->toots, NULL, &id, error)) {
    LOG_AND_ADD_ERROR(device->logger_ref, error,
                        "Cannot assign id  for toot. pid: %d", device->pid);
    return NULL;
  }

  cremona_toot_t *toot =
      create_toot(id, device->miner, device->pid, device->alloc, device->locker_factory, device->waiter_factory,
                  device->comm, wait, device->logger_ref, error);

  if (toot == NULL) {
    LOG_AND_ADD_ERROR(device->logger_ref, error,
                        "Cannot create toot. pid: %d", device->pid);
    id_mapper_remove(&device->toots, id, error);
    return NULL;
  }

  if (!id_mapper_replace(&device->toots, id, toot, error)) {
    LOG_AND_ADD_ERROR(device->logger_ref, error,
                        "Cannot publish device. pid: %d, miner: %d",
                        device->pid, device->miner);
    release_toot(toot);
    id_mapper_remove(&device->toots, id, error);
    return NULL;
  }
  return toot;
err:
  release_toot(toot);
  id_mapper_remove(&device->toots, id, error);
  return NULL;
}

bool recive_close_toot_result(cremona_toot_t *toot, cremona_device_t *device,
                              send_toot_result_t *message, crmna_err *error) {
  // if (toot->device != device) {
  //   LOG_AND_WRITE_ERROR(
  //       device->logger_ref, error,
  //       "toot is not in device. device id: %llu, device name: %s toot
  // id:
  //       %llu", device->miner, device->name, toot->id);
  //   return false;
  // }
  // destroy_toot(toot, error);
  //delete_toot(device, toot->id);
  return true;
}
