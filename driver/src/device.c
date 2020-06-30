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
              locker_factory_ref *locker_factory, communicator_ref *comm,
              waiter_factory_ref *waiter_factory,
              device_file_factory_ref *device_file_factory, logger *logger_ref,
              allocator_ref *alloc, crmna_err_t *error) {

  cremona_device_t *device =
      (cremona_device_t *)allocator_allocate(alloc, sizeof(cremona_device_t));
  if (device == NULL) {
    LOG_AND_WRITE_ERROR(logger_ref, error, "Cannot allocate device. pid: %d",
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
    LOG_AND_WRITE_ERROR(logger_ref, error,
                        "Cannot allocate locker in device. pid: %d", pid);
    goto err;
  }

  if (!create_id_mapper(id_mapper_factory, &device->toots, error)) {
    LOG_AND_WRITE_ERROR(logger_ref, error,
                        "Cannot allocate id_mapper in device. pid: %d", pid);
    goto err;
  }

  if (!create_device_file(device_file_factory, device, &device->device_file,
                          error)) {
    LOG_AND_WRITE_ERROR(logger_ref, error,
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
  if (count_toot(device) > 0) {
    LOG_WARN(device->logger_ref,
             "toot map is not empty but refCount is less or equal 0. "
             "{\"id\": %llu, "
             "\"pid\": %d, \"refCount\": %d}",
             device->miner, device->pid, device->refCount);
  }

  locker_unlock(&device->lock);
  device_file_free(&device->device_file);
  id_mapper_free(&device->toots);
  locker_free(&device->lock);
  allocator_free(device->alloc, device);

  LOG_INFO(device->logger_ref, "Device deallocated. name: %s id: %llu",
           device->name, device->miner);
}

cremona_toot_t *open_toot(cremona_device_t *device, bool wait,
                          crmna_err_t *error) {
  locker_lock(&device->lock);
  if (device->isDestroied) {
    locker_unlock(&device->lock);
    LOG_AND_WRITE_ERROR(
        device->logger_ref, error,
        "Device already destroyed. device id: %llu, device name: %s",
        device->miner, device->name);
    return NULL;
  }
  locker_unlock(&device->lock);

  int id = -1;
  if (!id_mapper_add_get_id(&device->toots, NULL, &id, error)) {
    LOG_AND_WRITE_ERROR(device->logger_ref, error,
                        "Cannot assign id  for toot. pid: %d", device->pid);
    return NULL;
  }

  cremona_toot_t *toot =
      create_toot(device, device->locker_factory, device->comm, device->waiter_factory, device->alloc, device->logger_ref, error);

  if (toot == NULL) {
    LOG_AND_WRITE_ERROR(device->logger_ref, error,
                        "Cannot create toot. pid: %d", device->pid);
    id_mapper_remove(&device->toots, id, error);
    return NULL;
  }

  if (!id_mapper_replace(&device->toots, id, toot, error)) {
    LOG_AND_WRITE_ERROR(device->logger_ref, error,
                        "Cannot publish device. pid: %d, miner: %d",
                        device->pid, device->miner);
    release_toot(toot);
    id_mapper_remove(&device->toots, id, error);
    return NULL;
  }

  new_toot_t new_toot = {toot->id, device->miner};
  char message[100];
  int msg_size = serialize_new_toot(&new_toot, message, sizeof(message));
  crmna_buf_t send_buf = {.buf = message, .buf_size = msg_size};
  toot_state_t state;
  if (communicator_send_message(device->comm, device->pid, CRMNA_NEW_TOOT,
                                &send_buf, error) == -1)
  {
    LOG_AND_WRITE_ERROR(device->logger_ref, error,
                        "Cannot send init device message. pid: %d",
                        device->pid);
    goto err;
  }

  if (wait) {
    wait_toot(toot, WAIT_OPEN);
  }
  locker_lock(&toot->lock);
  state = toot->state;
  locker_unlock(&toot->lock);

  if ((wait && state != OPEND) ||
      (!wait && state != OPEND && state != OPEN_RESULT_WAIT)) {
    LOG_AND_WRITE_ERROR(device->logger_ref, error,
                        "Cannot open toot. pid: %d", device->pid);
    goto err;
  }
  return add_ref_toot(toot);

err:
  release_toot(toot);
  id_mapper_remove(&device->toots, id, error);
  return NULL;
}

bool recive_close_toot_result(cremona_toot_t *toot, cremona_device_t *device,
                              send_toot_result_t *message, crmna_err_t *error) {
  // if (toot->device != device) {
  //   LOG_AND_WRITE_ERROR(
  //       device->logger_ref, error,
  //       "toot is not in device. device id: %llu, device name: %s toot
  // id:
  //       %llu", device->miner, device->name, toot->id);
  //   return false;
  // }
  destroy_toot(toot, error);
  delete_toot(device, toot->id);
  return true;
}
