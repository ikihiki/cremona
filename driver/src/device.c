#include "cremona_internal.h"
#include "message.h"

void cremona_device_lock(cremona_device_t *device) {
  LOG_ERROR(device->device_manager, " lock %p", device);
  device->device_manager->config.device_collbacks.lock(device);
}

void cremona_device_unlock(cremona_device_t *device) {
  LOG_ERROR(device->device_manager, " unlock %p", device);

  device->device_manager->config.device_collbacks.unlock(device);
}

static cremona_device_t *
call_init_device_cb(cremona_device_manager_t *device_manager, char *name, unsigned int miner) {
  return device_manager->config.device_collbacks.init_device(device_manager, name, miner);
}

static bool call_create_device_cb(cremona_device_manager_t *device_manager,
                                  cremona_device_t *device) {
  return device_manager->config.device_collbacks.create_device(device);
}

static bool call_destroy_device_cb(cremona_device_t *device) {
  return device->device_manager->config.device_collbacks.destroy_device(device);
}

cremona_device_t *add_ref_device(cremona_device_t *device) {
  if (device == NULL) {
    return NULL;
  }
  if (device->isDestroied) {
    LOG_ERROR(device->device_manager,
              "Device already destroyed. device id: %llu, device name: %s",
              device->id, device->name);
    return NULL;
  }

  device->refCount++;
  return device;
}

cremona_device_t *create_device(uint64_t id, uint32_t pid, uint32_t uid, char *name,
                                cremona_device_manager_t *device_manager,
                                crmna_err_t *error) {
  unsigned int miner;
  if (!rent_miner_num(device_manager , &miner)) {
    return NULL;
  }
    cremona_device_t *device =
      call_init_device_cb(device_manager, name, miner);

  if (device == NULL) {
    LOG_AND_WRITE_ERROR(device_manager, error, "Cannot init device. pid: %d",
                        pid);
    return NULL;
  }
  device->isDestroied = false;
  device->refCount = 0;
  device->id = id;
  device->pid = pid;
  device->uid = uid;
  //device->toots = kh_init(toot);
  memcpy(device->name, name, sizeof(device->name));
  device->device_manager = add_ref_device_manager(device_manager);

  if(!call_create_device_cb(device_manager, device)){
    release_device_manager(device_manager);
    call_destroy_device_cb(device);
    device_manager->config.device_collbacks.cleanup_device(device);
    LOG_AND_WRITE_ERROR(device_manager, error, "Cannot create device. pid: %d",
                        pid);
    return NULL;
  }

  LOG_ERROR(device->device_manager, "Device created. name: %s id: %ld pid: %u uid: %u",
            device->name, device->id, device->pid, device->uid);

  return device;
}

bool destroy_device(cremona_device_t *device, crmna_err_t *error) {

  cremona_device_lock(device);

  call_destroy_device_cb(device);
  device->isDestroied = true;

  bool toot_destroy_result = true;
  // for (k = kh_begin(device->toots); k != kh_end(device->toots); ++k) {
  //   if (kh_exist(device->toots, k)) {
  //     if (!destroy_toot(kh_value(device->toots, k), error)) {
  //       LOG_ERROR(device->device_manager, "Destroy Error error: %s",
  //                 error->error_msg);
  //       toot_destroy_result = false;
  //     }
  //   }
  // }
  LOG_AND_WRITE_ERROR(device->device_manager, error,
                      "Destroy toot error: device id: %llu", device->id);

  LOG_INFO(device->device_manager, "Device destroyed. name: %s id: %ld pid: %u",
           device->name, device->id, device->pid);

  cremona_device_unlock(device);
  return toot_destroy_result;
}

void release_device(cremona_device_t *device) {
  if(device == NULL){
    return;
  }
  cremona_device_lock(device);
  device->refCount--;
  cremona_device_unlock(device);
  cremona_device_manager_t *device_manager = device->device_manager;
  if (device->refCount <= 0) {
    if (device->refCount < 0) {
      LOG_WARN(
          device_manager,
          "refCount is under 0 {\"id\": %ld, \"pid\": %d, \"refCount\": %d}",
          device->id, device->pid, device->refCount);
    }
    if (device->isDestroied == false) {
      LOG_WARN(device_manager,
               "not destroied but refCount is less or equal 0. {\"id\": %llu, "
               "\"pid\": %d, \"refCount\": %d}",
               device->id, device->pid, device->refCount);
    }
    if (count_toot(device) > 0) {
      LOG_WARN(device_manager,
               "toot map is not empty but refCount is less or equal 0. "
               "{\"id\": %llu, "
               "\"pid\": %d, \"refCount\": %d}",
               device->id, device->pid, device->refCount);
    }
    release_device_manager(device->device_manager);
    device_manager->config.device_collbacks.cleanup_device(device);
    LOG_INFO(device_manager, "Device deallocated. name: %s id: %llu",
             device->name, device->id);
  }
}

cremona_toot_t *open_toot(cremona_device_t *device, bool wait,
                          crmna_err_t *err) {

  if (device->isDestroied) {
    LOG_AND_WRITE_ERROR(
        device->device_manager, err,
        "Device already destroyed. device id: %llu, device name: %s",
        device->id, device->name);
    return NULL;
  }

  cremona_toot_t *toot = create_toot(device, err);

  if (toot == NULL) {
    return NULL;
  }

  add_toot(device, toot);

  new_toot_t new_toot = {toot->id, device->id};
  char message[100];
  int msg_size = serialize_new_toot(&new_toot, message, sizeof(message));
  send_message(device->device_manager, device->pid, CRMNA_NEW_TOOT, message,
               msg_size);

  if (wait) {
    wait_toot(toot, WAIT_OPEN);
  }

  return add_ref_toot(toot);
}

bool recive_close_toot_result(cremona_toot_t *toot, cremona_device_t *device,
                              send_toot_result_t *message, crmna_err_t *error) {
  if (toot->device != device) {
    LOG_AND_WRITE_ERROR(
        device->device_manager, error,
        "toot is not in device. device id: %llu, device name: %s toot id: %llu",
        device->id, device->name, toot->id);
    return false;
  }
  destroy_toot(toot, error);
  delete_toot(device, toot->id);
  return true;
}
