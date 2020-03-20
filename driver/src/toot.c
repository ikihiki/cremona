#include "cremona_internal.h"
#include "message.h"

void cremona_toot_lock(cremona_toot_t *toot) {
  LOG_ERROR(toot->device->device_manager,
            " lock toot device id: %llu toot id: %llu", toot->device->id,
            toot->id);
  toot->device->device_manager->config.toot_callbacks.lock(toot);
}
void cremona_toot_unlock(cremona_toot_t *toot) {
  LOG_ERROR(toot->device->device_manager,
            " unlock toot device id: %llu toot id: %llu", toot->device->id,
            toot->id);
  toot->device->device_manager->config.toot_callbacks.unlock(toot);
}

void wait_toot(cremona_toot_t *toot, toot_wait_type_t wait_type) {
  toot->device->device_manager->config.toot_callbacks.wait(toot, wait_type);
}

void notify(cremona_toot_t *toot) {
  toot->device->device_manager->config.toot_callbacks.notify(toot);
}

cremona_toot_t *add_ref_toot(cremona_toot_t *toot) {
  if (toot == NULL) {
    return NULL;
  }
  toot->refCount++;
  return toot;
}

cremona_toot_t *create_toot(cremona_device_t *device, crmna_err_t *error) {
  cremona_toot_t *toot =
      device->device_manager->config.toot_callbacks.create_toot();

  if (toot == NULL) {
    LOG_AND_WRITE_ERROR(device->device_manager, error,
                        "Create toot fail. device: %s", device->name);
    return NULL;
  }

  toot->id = create_id(device->device_manager);
  toot->prev_count = 0;
  toot->send_count = 0;
  toot->refCount = 0;
  toot->state = OPEN_RESULT_WAIT;
  toot->device = add_ref_device(device);
  return toot;
}

bool recive_open_toot_result(cremona_toot_t *toot, new_toot_result_t *message,
                             crmna_err_t *err) {
  if (message->result != 0) {
    LOG_AND_WRITE_ERROR(toot->device->device_manager, err,
                        "Create new toot failed. device: %s toot id: %llu",
                        toot->device->name, toot->id);
    toot->state = TOOT_ERROR;
    return false;
  }
  cremona_toot_lock(toot);
  toot->state = OPEND;
  notify(toot);
  cremona_toot_unlock(toot);
  return true;
}

bool close_toot(cremona_toot_t *toot, crmna_err_t *error) {
  if (toot->device->isDestroied) {
    LOG_AND_WRITE_ERROR(
        toot->device->device_manager, error,
        "Device already destroyed. device id: %llu, device name: %s",
        toot->device->id, toot->device->name);
    return false;
  }

  cremona_toot_lock(toot);

  if (toot->state != OPEND) {
    cremona_toot_unlock(toot);
    return false;
  }

  toot->state = CLOSE_RESULT_WAIT;

  send_toot_t send_toot = {toot->id, toot->device->id};
  char message[100];
  int msg_size = serialize_send_toot(&send_toot, message, sizeof(message));
  send_message(toot->device->device_manager, toot->device->pid, CRMNA_SEND_TOOT,
               message, msg_size);

  cremona_toot_unlock(toot);
  wait_toot(toot, WAIT_CLOSE);

  return true;
}

bool destroy_toot(cremona_toot_t *toot, crmna_err_t *error) {
  cremona_toot_lock(toot);
  toot->device->device_manager->config.toot_callbacks.destroy_toot(toot);
  toot->state = DESTROYED;
  cremona_toot_unlock(toot);

  notify(toot);
  return true;
}

void release_toot(cremona_toot_t *toot) {
  if (toot == NULL) {
    return;
  }

  cremona_device_manager_t *device_manager = toot->device->device_manager;
  cremona_toot_lock(toot);
  toot->refCount--;
  cremona_toot_unlock(toot);
  if (toot->refCount <= 0) {
    if (toot->refCount < 0) {
      LOG_WARN(device_manager,
               "refCount is under 0 {\"toot id\": %ld, "
               "\"refCount\": %d}",
               toot->id, toot->refCount);
    }
    if (toot->state != DESTROYED) {
      LOG_WARN(device_manager,
               "not destroied but refCount is less or equal 0. {"
               "\"toot id\": %d, \"refCount\": %d \"state\": %d}",
               toot->id, toot->refCount, toot->state);
    }
    release_device(toot->device);
    toot->device = NULL;
    toot->state = CREANUPED;
    device_manager->config.toot_callbacks.cleanup_toot(toot);
    LOG_INFO(device_manager, "Toot deallocated. id: %llu", toot->id);
  }
}

bool add_toot_text(cremona_toot_t *toot, char *text, bool wait,
                   crmna_err_t *err) {
  cremona_toot_lock(toot);
  if (toot->state != OPEND) {
    LOG_AND_WRITE_ERROR(toot->device->device_manager, err,
                        "Invalid state. device id: %llu, toot id %llu",
                        toot->device->id, toot->id);
    cremona_toot_unlock(toot);
    return false;
  }

  toot->state = ADD_TEXT_RESULT_WAIT;
  toot->send_count = strlen(text);
  add_toot_text_t add_toot_text = {toot->id, toot->device->id, text};
  char message[100];
  int msg_size =
      serialize_add_toot_text(&add_toot_text, message, sizeof(message));
  send_message(toot->device->device_manager, toot->device->pid,
               CRMNA_ADD_TOOT_TEXT, message, msg_size);
  cremona_toot_unlock(toot);

  if (wait) {
    wait_toot(toot, WAIT_WRITE);
  }

  return true;
}

bool recive_add_toot_text_result(cremona_toot_t *toot,
                                 add_toot_text_result_t *message,
                                 crmna_err_t *err) {
  if (message->result != toot->send_count) {
    LOG_AND_WRITE_ERROR(toot->device->device_manager, err,
                        "Add  toot text failed. device: %s toot id: %llu",
                        toot->device->name, toot->id);
    toot->state = TOOT_ERROR;
    return false;
  }
  cremona_toot_lock(toot);
  toot->state = WRITE_COMPLEATE;
  toot->prev_count = toot->send_count;
  toot->send_count = 0;
  notify(toot);
  cremona_toot_unlock(toot);
  return true;
}
