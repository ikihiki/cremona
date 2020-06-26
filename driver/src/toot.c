#include "cremona_internal.h"
#include "message.h"

cremona_toot_t *add_ref_toot(cremona_toot_t *toot) {
  if (toot == NULL) {
    return NULL;
  }
  toot->refCount++;
  return toot;
}

cremona_toot_t *create_toot(cremona_device_t *device, crmna_err_t *error) {
  cremona_toot_t *toot;
  if (toot == NULL)
  {
    LOG_AND_WRITE_ERROR(device->logger_ref, error,
                        "Create toot fail. device: %s", device->name);
    return NULL;
  }

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
    LOG_AND_WRITE_ERROR(toot->logger_ref, err,
                        "Create new toot failed. device: %s toot id: %llu",
                        toot->device->name, toot->id);
    toot->state = TOOT_ERROR;
    return false;
  }
  //cremona_toot_lock(toot);
  toot->state = OPEND;
  //notify(toot);
  //cremona_toot_unlock(toot);
  return true;
}

bool close_toot(cremona_toot_t *toot, crmna_err_t *error) {
  if (toot->device->isDestroied) {
    LOG_AND_WRITE_ERROR(
        toot->logger_ref, error,
        "Device already destroyed. device id: %llu, device name: %s",
        toot->device->miner, toot->device->name);
    return false;
  }

//  cremona_toot_lock(toot);

  if (toot->state != OPEND) {
//    cremona_toot_unlock(toot);
    return false;
  }

  toot->state = CLOSE_RESULT_WAIT;

  send_toot_t send_toot = {toot->id, toot->device->miner};
  char message[100];
  int msg_size = serialize_send_toot(&send_toot, message, sizeof(message));
  // send_message(toot->device->device_manager, toot->device->pid, CRMNA_SEND_TOOT,
  //              message, msg_size);

//  cremona_toot_unlock(toot);
  //wait_toot(toot, WAIT_CLOSE);

  return true;
}

bool destroy_toot(cremona_toot_t *toot, crmna_err_t *error) {
  //cremona_toot_lock(toot);
  toot->state = DESTROYED;
  //cremona_toot_unlock(toot);

  //notify(toot);
  return true;
}

void release_toot(cremona_toot_t *toot) {
  if (toot == NULL) {
    return;
  }

//  cremona_toot_lock(toot);
  toot->refCount--;
  //cremona_toot_unlock(toot);
  if (toot->refCount <= 0) {
    if (toot->refCount < 0) {
      LOG_WARN(toot->logger_ref,
               "refCount is under 0 {\"toot id\": %ld, "
               "\"refCount\": %d}",
               toot->id, toot->refCount);
    }
    if (toot->state != DESTROYED) {
      LOG_WARN(toot->logger_ref,
               "not destroied but refCount is less or equal 0. {"
               "\"toot id\": %d, \"refCount\": %d \"state\": %d}",
               toot->id, toot->refCount, toot->state);
    }
    release_device(toot->device);
    toot->device = NULL;
    toot->state = CREANUPED;
    LOG_INFO(toot->logger_ref, "Toot deallocated. id: %llu", toot->id);
  }
}

bool add_toot_text(cremona_toot_t *toot, char *text, bool wait,
                   crmna_err_t *err) {
 // cremona_toot_lock(toot);
  if (toot->state != OPEND) {
    LOG_AND_WRITE_ERROR(toot->logger_ref, err,
                        "Invalid state. device id: %llu, toot id %llu",
                        toot->device->miner, toot->id);
    //cremona_toot_unlock(toot);
    return false;
  }

  toot->state = ADD_TEXT_RESULT_WAIT;
  toot->send_count = strlen(text);
  add_toot_text_t add_toot_text = {toot->id, toot->device->miner, text};
  char message[100];
  int msg_size =
      serialize_add_toot_text(&add_toot_text, message, sizeof(message));
  // send_message(toot->device->device_manager, toot->device->pid,
  //              CRMNA_ADD_TOOT_TEXT, message, msg_size);
  //cremona_toot_unlock(toot);

  if (wait) {
//    wait_toot(toot, WAIT_WRITE);
  }

  return true;
}

bool recive_add_toot_text_result(cremona_toot_t *toot,
                                 add_toot_text_result_t *message,
                                 crmna_err_t *err) {
  if (message->result != toot->send_count) {
    LOG_AND_WRITE_ERROR(toot->logger_ref, err,
                        "Add  toot text failed. device: %s toot id: %llu",
                        toot->device->name, toot->id);
    toot->state = TOOT_ERROR;
    return false;
  }
  //cremona_toot_lock(toot);
  toot->state = WRITE_COMPLEATE;
  toot->prev_count = toot->send_count;
  toot->send_count = 0;
  //notify(toot);
  //cremona_toot_unlock(toot);
  return true;
}
