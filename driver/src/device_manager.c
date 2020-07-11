#include "message.h"

#include "cremona_internal.h"

static void clear_all(cremona_device_manager_t *devive_manager) {
  clear_communicate(&devive_manager->comm);
  clear_id_mapper_ref(&devive_manager->devices);
  clear_locker_ref(&devive_manager->lock);
}

cremona_device_manager_t *
add_ref_device_manager(cremona_device_manager_t *device_manager) {
  device_manager->refCount++;
  return device_manager;
}

void release_device_manager(cremona_device_manager_t *device_manager) {
  device_manager->refCount--;
}

bool create_device_message(cremona_device_manager_t *device_manager,
                                  create_device_t *recive_message, int pid,
                                  create_device_result_t *result_message,
                                  crmna_err *error) {

  int miner = -1;
  if (!id_mapper_add_get_id(&device_manager->devices, NULL, &miner, error)) {
    LOG_AND_ADD_ERROR(device_manager->logger_ref, error,
                      "Cannot assign miner number for device. pid: %d", pid);
    return false;
  }

  cremona_device_t *device = create_device(
      miner, pid, recive_message->uid, recive_message->name,
      device_manager->id_mapper_factory, device_manager->locker_factory,
      &device_manager->comm, device_manager->waiter_factory,
      device_manager->device_file_factory, device_manager->logger_ref,
      device_manager->alloc, error);
  if (device == NULL) {
    id_mapper_remove(&device_manager->devices, miner, error);
    LOG_AND_ADD_ERROR(device_manager->logger_ref, error,
                      "Cannot create device. pid: %d", pid);
    return false;
  }

  if (!id_mapper_replace(&device_manager->devices, miner, device, error)) {
    release_device(device);
    id_mapper_remove(&device_manager->devices, miner, error);
    LOG_AND_ADD_ERROR(device_manager->logger_ref, error,
                      "Cannot publish device. pid: %d, miner: %d", pid, miner);
    return false;
  }

  LOG_INFO(device_manager->logger_ref, "Device created. name: %s id: %ld",
           device->name, device->miner);

  result_message->id = device->miner;
  return true;
}

bool destroy_device_message(cremona_device_manager_t *device_manager,
                                   destroy_device_t *recive_message, int pid,
                                   destroy_device_result_t *result_message,
                                   crmna_err *error) {

  cremona_device_t *device = (cremona_device_t *)id_mapper_find(
      &device_manager->devices, recive_message->id, error);
  if (device == NULL) {
    LOG_AND_ADD_ERROR(device_manager->logger_ref, error,
                      "Cannot find device of %llu.", recive_message->id);
    return false;
  }
  add_ref_device(device);

  LOG_ERROR(device_manager->logger_ref, "%p  %d %d %d", device_manager,
            device->miner, device->pid, device->refCount);

  if (device->pid != pid) {
    LOG_AND_ADD_ERROR(device_manager->logger_ref, error,
                      "Deffecent pid. id: %llu reqest: %d target: %d",
                      device->miner, pid, device->pid);
    release_device(device);
    return false;
  }

  if (!id_mapper_remove(&device_manager->devices, device->miner, error)) {
    LOG_AND_ADD_ERROR(device_manager->logger_ref, error,
                      "Cannot remove device of %d.", device->pid);
    release_device(device);
    return false;
  }
  release_device(device); // id_mapperからの参照削除

  destroy_device(device);
  release_device(device);

  result_message->id = device->miner;
  return true;
}

bool init_device_manager(cremona_device_manager_t *device_manager,
                         int driver_number_min, int driver_number_max,
                         communicator_factory_ref *communicator_factory,
                         locker_factory_ref *locker_factory,
                         id_mapper_factory_ref *id_mapper_factory,
                         waiter_factory_ref *waiter_factory,
                         device_file_factory_ref *device_file_factory,
                         logger *logger, allocator_ref *alloc, crmna_err *err) {
  clear_all(device_manager);

  device_manager->driver_number_min = driver_number_min;
  device_manager->driver_number_max = driver_number_max;
  device_manager->locker_factory = locker_factory;
  device_manager->id_mapper_factory = id_mapper_factory;
  device_manager->waiter_factory = waiter_factory;
  device_manager->device_file_factory = device_file_factory;
  device_manager->refCount = 0;
  device_manager->logger_ref = logger;
  device_manager->alloc = alloc;


  if (!create_locker(locker_factory, &device_manager->lock, err)) {
    return false;
  }
  if (!create_id_mapper_range(id_mapper_factory, &device_manager->devices,
                              driver_number_min, driver_number_max, err)) {
    locker_free(&device_manager->lock);
    return false;
  }
  
  if (!init_communicate(&device_manager->comm, communicator_factory,
                        locker_factory, id_mapper_factory, device_manager,
                        logger, err)) {
    locker_free(&device_manager->lock);
    id_mapper_free(&device_manager->devices);
    return false;
  }
  return true;
}

bool destroy_device_manager(cremona_device_manager_t *device_manager,
                            crmna_err *err) {

  locker_lock(&device_manager->lock);

  id_mapper_iterator_ref iterator;
  cremona_device_t *device;
  if (!id_mapper_get_iterator(&device_manager->devices, &iterator, err)) {
    locker_unlock(&device_manager->lock);
    return false;
  }

  while (id_mapper_iterator_next(&iterator, err)) {
    device = (cremona_device_t *)id_mapper_iterator_get_value(&iterator, err);
    if (HAS_ERROR(err)) {
      locker_unlock(&device_manager->lock);
      return false;
    }
    if (device == NULL)
      continue;

    destroy_device(device);
  }

  if (HAS_ERROR(err)) {
    locker_unlock(&device_manager->lock);
    return false;
  }

  id_mapper_iterator_free(&iterator);
  id_mapper_free(&device_manager->devices);
  destory_communicate(&device_manager->comm);

  locker_unlock(&device_manager->lock);
  locker_free(&device_manager->lock);
  return true;
}

bool get_stats(cremona_device_manager_t *device_manager, uint32_t pid) {
  get_stats_result_t result;
  result.total_device_count = 0;

  //device_manager->config.devicce_manager_callbacks.lock(device_manager);
  // for (k = kh_begin(device_manager->devices);
  //      k != kh_end(device_manager->devices); ++k) {
  //   if (kh_exist(device_manager->devices, k)) {
  //     result.total_device_count++;
  //   }
  // }
  //device_manager->config.devicce_manager_callbacks.unlock(device_manager);

  char message[100];
  size_t size = serialize_get_stats_result(&result, message, sizeof(message));
  // send_message(device_manager, pid, CRMNA_GET_STATS_RESULT, message, size);
  return true;
}
