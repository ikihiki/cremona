#include "message.h"

#include "cremona_internal.h"

static void clear_all(cremona_device_manager_t *devive_manager) {
  clear_communicator_ref(&devive_manager->comm);
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

bool init_device_manager(cremona_device_manager_t *device_manager,
                         int driver_number_min, int driver_number_max,
                         communicator_factory_ref *communicator_factory,
                         locker_factory_ref *locker_factory,
                         id_mapper_factory_ref *id_mapper_factory,
                         waiter_factory_ref *waiter_factory,
                         device_file_factory_ref *device_file_factory,
                         logger *logger, allocator_ref *alloc,
                         crmna_err_t *err) {
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

  if (!create_communicator(communicator_factory, device_manager,
                           &device_manager->comm, err)) {
    return false;
  }
  if (!create_locker(locker_factory, &device_manager->lock, err)) {
    communicator_free(&device_manager->comm);
    return false;
  }
  if (!create_id_mapper_range(id_mapper_factory, &device_manager->devices,
                              driver_number_min, driver_number_max, err)) {
    communicator_free(&device_manager->comm);
    locker_free(&device_manager->lock);
    return false;
  }
  return true;
}

bool destroy_device_manager(cremona_device_manager_t *device_manager,
                            crmna_err_t *err) {

  locker_lock(&device_manager->lock);

  id_mapper_iterator_ref iterator;
  cremona_device_t *device;
  if (!id_mapper_get_iterator(&device_manager->devices, &iterator, err)) {
    locker_unlock(&device_manager->lock);
    return false;
  }

  while (id_mapper_iterator_next(&iterator, err)) {
    device = (cremona_device_t *)id_mapper_iterator_get_value(&iterator, err);
    if (err->error_msg_len != 0) {
      locker_unlock(&device_manager->lock);
      return false;
    }
    if (device == NULL)
      continue;

    destroy_device(device);
  }

  if (err->error_msg_len != 0) {
    locker_unlock(&device_manager->lock);
    return false;
  }

  id_mapper_iterator_free(&iterator);
  id_mapper_free(&device_manager->devices);
  communicator_free(&device_manager->comm);

  locker_unlock(&device_manager->lock);
  locker_free(&device_manager->lock);
  return true;
}

bool get_stats(cremona_device_manager_t *device_manager, uint32_t pid) {
  get_stats_result_t result;
  result.total_device_count = 0;

  device_manager->config.devicce_manager_callbacks.lock(device_manager);
  // for (k = kh_begin(device_manager->devices);
  //      k != kh_end(device_manager->devices); ++k) {
  //   if (kh_exist(device_manager->devices, k)) {
  //     result.total_device_count++;
  //   }
  // }
  device_manager->config.devicce_manager_callbacks.unlock(device_manager);

  char message[100];
  size_t size = serialize_get_stats_result(&result, message, sizeof(message));
  // send_message(device_manager, pid, CRMNA_GET_STATS_RESULT, message, size);
  return true;
}

static bool create_device_message(cremona_device_manager_t *device_manager,
                                  uint32_t pid, crmna_buf_t *buf,
                                  crmna_err_t *error) {
  create_device_t data;
  if (!deserialize_create_conn(buf->buf, buf->buf_size, &data)) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot decode create device message. pid: %d", pid);
    return false;
  }

  int miner = -1;
  if (!id_mapper_add_get_id(&device_manager->devices, NULL, &miner, error)) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot assign miner number for device. pid: %d", pid);
    return false;
  }

  cremona_device_t *device = create_device(
      miner, pid, data.uid, data.name,
      device_manager->id_mapper_factory, device_manager->locker_factory,
      &device_manager->comm, device_manager->waiter_factory,
      device_manager->device_file_factory, device_manager->logger_ref,
      device_manager->alloc, error);
  if (device == NULL)
  {
    id_mapper_remove(&device_manager->devices, miner, error);
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot create device. pid: %d", pid);
    return false;
  }

  create_device_result_t result_message = {device->miner};
  char message[100];
  int msg_size =
      serialize_create_device_result(&result_message, message, sizeof(message));
  crmna_buf_t send_buf = {.buf = message, .buf_size = msg_size};

  if (communicator_send_message(&device_manager->comm, pid,
                                CRMNA_CREATE_DEVICE_RESULT, &send_buf,
                                error) == -1) {
    release_device(device);
    id_mapper_remove(&device_manager->devices, miner, error);
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot send init device message. pid: %d", pid);
    return false;
  }

  if (!id_mapper_replace(&device_manager->devices, miner, device, error)) {
    release_device(device);
    id_mapper_remove(&device_manager->devices, miner, error);
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot publish device. pid: %d, miner: %d", pid,
                        miner);
    return false;
  }

  LOG_INFO(device_manager->logger_ref, "Device created. name: %s id: %ld",
           device->name, device->miner);
  return true;
}

bool destroy_device_message(cremona_device_manager_t *device_manager,
                            uint32_t pid, crmna_buf_t *buf,
                            crmna_err_t *error) {
  destroy_device_t msg;
  if (!deserialize_destroy_device(buf->buf, buf->buf_size, &msg)) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot deserialize message. pid: %d", pid);
    return false;
  }

  cremona_device_t *device = (cremona_device_t *)id_mapper_find(
      &device_manager->devices, msg.id, error);
  if (device == NULL) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot find device of %llu.", msg.id);
    return false;
  }
  add_ref_device(device);

  LOG_ERROR(device_manager->logger_ref, "%p  %d %d %d", device_manager,
            device->miner, device->pid, device->refCount);

  if (device->pid != pid) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Deffecent pid. id: %llu reqest: %d target: %d",
                        device->miner, pid, device->pid);
    release_device(device);
    return false;
  }

  if (!id_mapper_remove(&device_manager->devices, device->miner, error)) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot remove device of %d.", device->pid);
    release_device(device);
    return false;
  }

  release_device(device); //id_mapperからの参照削除

  destroy_device(device);

  destroy_device_result_t result = {device->miner};
  char message[100];
  int msg_size =
      serialize_destroy_device_result(&result, message, sizeof(message));
  crmna_buf_t send_buf = {.buf = message, .buf_size = msg_size};
  if (communicator_send_message(&device_manager->comm, pid,
                                CRMNA_DESTROY_DEVICE_RESULT, &send_buf,
                                error) == -1) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot send destroy device of %d.", device->pid);
    release_device(device);
    return false;
  }

  release_device(device);
  return true;
}

bool new_toot_result_message(cremona_device_manager_t *device_manager,
                             uint32_t pid, crmna_buf_t *buf,
                             crmna_err_t *error) {
  new_toot_result_t msg;
  bool result = true;
  cremona_device_t *device = NULL;
  cremona_toot_t *toot = NULL;

  if (!deserialize_new_toot_result(buf->buf, buf->buf_size, &msg)) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot deserialize message. pid: %d", pid);
    result = false;
    goto end;
  }

  LOG_ERROR(device_manager->logger_ref,
            "Recive msg  device id: %llu toot id: %llu", msg.device_id,
            msg.toot_id);

  // device = cremna_get_device(device_manager, msg.device_id);
  if (device == NULL) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot find device. id: %llu", msg.device_id);
    result = false;
    goto end;
  }

  toot = cremna_get_toot(device, msg.toot_id);
  if (toot == NULL) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot find toot. id: %llu", msg.toot_id);
    result = false;
    goto end;
  }

  if (!recive_open_toot_result(toot, &msg, error)) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Failed open toot. device id: %llu toot id: %llu",
                        msg.device_id, msg.toot_id);
    result = false;
    goto end;
  }

end:
  release_toot(toot);
  release_device(device);

  return result;
}

bool send_toot_result_message(cremona_device_manager_t *device_manager,
                              uint32_t pid, crmna_buf_t *buf,
                              crmna_err_t *error) {
  send_toot_result_t msg;
  bool result = true;
  cremona_device_t *device = NULL;
  cremona_toot_t *toot = NULL;

  if (!deserialize_send_toot_result(buf->buf, buf->buf_size, &msg)) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot deserialize message. pid: %d", pid);
    result = false;
    goto end;
  }

  // device = cremna_get_device(device_manager, msg.device_id);
  if (device == NULL) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot find device. id: %llu", msg.device_id);
    result = false;
    goto end;
  }

  toot = cremna_get_toot(device, msg.toot_id);
  if (toot == NULL) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot find toot. id: %llu", msg.toot_id);
    result = false;
    goto end;
  }

  if (!recive_close_toot_result(toot, device, &msg, error)) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Failed close toot. device id: %llu toot id: %llu",
                        msg.device_id, msg.toot_id);
    result = false;
    goto end;
  }

end:
  release_toot(toot);
  release_device(device);

  return result;
}

bool add_toot_text_result_message(cremona_device_manager_t *device_manager,
                                  uint32_t pid, crmna_buf_t *buf,
                                  crmna_err_t *error) {
  add_toot_text_result_t msg;
  bool result = true;
  cremona_device_t *device = NULL;
  cremona_toot_t *toot = NULL;

  if (!deserialize_add_toot_text_result(buf->buf, buf->buf_size, &msg)) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot deserialize message. pid: %d", pid);
    result = false;
    goto end;
  }

  // device = cremna_get_device(device_manager, msg.device_id);
  if (device == NULL) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot find device. id: %llu", msg.device_id);
    result = false;
    goto end;
  }

  toot = cremna_get_toot(device, msg.toot_id);
  if (toot == NULL) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Cannot find toot. id: %llu", msg.toot_id);
    result = false;
    goto end;
  }

  if (!recive_add_toot_text_result(toot, &msg, error)) {
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Failed add toot text. device id: %llu toot id: %llu",
                        msg.device_id, msg.toot_id);
    result = false;
    goto end;
  }

end:
  release_toot(toot);
  release_device(device);

  return result;
}

bool reciveMessage(void *obj, uint32_t pid, int type, crmna_buf_t *buf,
                   crmna_err_t *error) {
  cremona_device_manager_t *device_manager = (cremona_device_manager_t *)obj;
  switch (type) {
  case CRMNA_GET_STATS:
    return get_stats(device_manager, pid);
  case CRMNA_CREATE_DEVICE:
    return create_device_message(device_manager, pid, buf, error);
  case CRMNA_DESTROY_DEVICE:
    return destroy_device_message(device_manager, pid, buf, error);
  case CRMNA_NEW_TOOT_RESULT:
    return new_toot_result_message(device_manager, pid, buf, error);
  case CRMNA_SEND_TOOT_RESULT:
    return send_toot_result_message(device_manager, pid, buf, error);
  case CRMNA_ADD_TOOT_TEXT_RESULT:
    return add_toot_text_result_message(device_manager, pid, buf, error);

  default:
    LOG_AND_WRITE_ERROR(device_manager->logger_ref, error,
                        "Unknown message type. type: %d pid: %u", type, pid);
    return false;
  }
}
