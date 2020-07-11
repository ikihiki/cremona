#include "cremona_internal.h"
#include "message.h"
#include "communicate.h"

cremona_toot_t *add_ref_toot(cremona_toot_t *toot) {
  if (toot == NULL) {
    return NULL;
  }
  toot->refCount++;
  return toot;
}

static void destroy_toot(cremona_toot_t *toot) {
  if (toot == NULL) {
    return;
  }

  if (toot->lock.obj != NULL) {
    locker_free(&toot->lock);
  }

  if (toot->wait.obj != NULL) {
    waiter_free(&toot->wait);
  }

  allocator_ref *alloc = toot->alloc;
  allocator_free(alloc, toot);
}

cremona_toot_t *create_toot(int id, int miner, int pid, allocator_ref *alloc,
                            locker_factory_ref *locker_factory,
                            waiter_factory_ref *waiter_factory,
                            communicate_t *comm,
                            bool wait,
                             logger *logger_ref,
                            crmna_err *error) {

  cremona_toot_t *toot =
      (cremona_toot_t *)allocator_allocate(alloc, sizeof(cremona_toot_t));
  if (toot == NULL) {
    LOG_AND_ADD_ERROR(logger_ref, error, "Create toot fail.");
    return NULL;
  }
  toot->id = id;
  toot->miner = miner;
  toot->pid = pid;
  toot->alloc = alloc;
  toot->logger_ref = logger_ref;
  toot->prev_count = 0;
  toot->send_count = 0;
  toot->refCount = 0;
  toot->state = OPEN_RESULT_WAIT;
  clear_locker_ref(&toot->lock);
  clear_waiter_ref(&toot->wait);

  if (!create_locker(locker_factory, &toot->lock, error)) {
    destroy_toot(toot);
    return NULL;
  }
  if(!create_waiter(waiter_factory, &toot->wait, error)){
    destroy_toot(toot);
    return NULL;
  }
  if(comm == NULL){
    destroy_toot(toot);
    LOG_AND_ADD_ERROR(logger_ref, error, "comm is null")
    
    return NULL;
  }
  toot->comm = comm;

  // 参照を他に渡すので、初期化関数が保持していることを明示する。
  add_ref_toot(toot);
  new_toot_t new_toot = {toot->id, toot->miner};
  if (!send_new_toot(toot->comm, toot->pid, &new_toot, toot, error)) {
    destroy_toot(toot);
    LOG_AND_ADD_ERROR(toot->logger_ref, error,
                      "Cannot send new toot message.  pid: %d", toot->pid);
    
    return NULL;
  }

  if (wait) {

    wait_toot(toot, WAIT_OPEN);
  }
  locker_lock(&toot->lock);
  bool fail =
      (wait && toot->state != OPEND) ||
      (!wait && toot->state != OPEND && toot->state != OPEN_RESULT_WAIT);
  locker_unlock(&toot->lock);

  if (fail) {
    cancel_wait_new_toot_result(toot->comm, &new_toot);
    destroy_toot(toot);
    LOG_AND_ADD_ERROR(toot->logger_ref, error, "Cannot open toot. pid: %d",
                      toot->pid);
    
    return NULL;
  }
  //一瞬参照が外れてcleanupが走らないように
  toot->refCount--;
  return toot;
}

bool recive_open_toot_result(cremona_toot_t *toot, new_toot_result_t *message,
                             crmna_err *err) {
  if (!message->result) {
    LOG_AND_ADD_ERROR(toot->logger_ref, err,
                        "Create new toot failed. "JSON_OUT(miner, d, toot_id, d),
                        toot->miner, toot->id);
    locker_lock(&toot->lock);
    toot->state = TOOT_ERROR;
    locker_unlock(&toot->lock);
    return false;
  }

  locker_lock(&toot->lock);
  toot->state = OPEND;
  locker_unlock(&toot->lock);
  waiter_notify(&toot->wait);
  return true;
}

bool close_toot(cremona_toot_t *toot, crmna_err *error) {
  // if (toot->device->isDestroied) {
  //   LOG_AND_WRITE_ERROR(
  //       toot->logger_ref, error,
  //       "Device already destroyed. device id: %llu, device name: %s",
  //       toot->device->miner, toot->device->name);
  //   return false;
  // }

  //  cremona_toot_lock(toot);

  if (toot->state != OPEND) {
    //    cremona_toot_unlock(toot);
    return false;
  }

  toot->state = CLOSE_RESULT_WAIT;

  // send_toot_t send_toot = {toot->id, toot->device->miner};
  char message[100];
  // int msg_size = serialize_send_toot(&send_toot, message, sizeof(message));
  // send_message(toot->device->device_manager, toot->device->pid,
  // CRMNA_SEND_TOOT,
  //              message, msg_size);

  //  cremona_toot_unlock(toot);
  // wait_toot(toot, WAIT_CLOSE);

  return true;
}

void release_toot(cremona_toot_t *toot) {
  if (toot == NULL) {
    return;
  }

  //  cremona_toot_lock(toot);
  toot->refCount--;
  // cremona_toot_unlock(toot);
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
    // release_device(toot->device);
    // toot->device = NULL;
    toot->state = CREANUPED;
    LOG_INFO(toot->logger_ref, "Toot deallocated. id: %llu", toot->id);
  }
}

bool add_toot_text(cremona_toot_t *toot, char *text, bool wait,
                   crmna_err *err) {
  // cremona_toot_lock(toot);
  if (toot->state != OPEND) {
    // LOG_AND_WRITE_ERROR(toot->logger_ref, err,
    //                     "Invalid state. device id: %llu, toot id %llu",
    //                     toot->device->miner, toot->id);
    // cremona_toot_unlock(toot);
    return false;
  }

  toot->state = ADD_TEXT_RESULT_WAIT;
  toot->send_count = strlen(text);
  // add_toot_text_t add_toot_text = {toot->id, toot->device->miner, text};
  char message[100];
  // int msg_size =
  //     serialize_add_toot_text(&add_toot_text, message, sizeof(message));
  // send_message(toot->device->device_manager, toot->device->pid,
  //              CRMNA_ADD_TOOT_TEXT, message, msg_size);
  // cremona_toot_unlock(toot);

  if (wait) {
    //    wait_toot(toot, WAIT_WRITE);
  }

  return true;
}

bool recive_add_toot_text_result(cremona_toot_t *toot,
                                 add_toot_text_result_t *message,
                                 crmna_err *err) {
  if (message->result != toot->send_count) {
    // LOG_AND_WRITE_ERROR(toot->logger_ref, err,
    //                     "Add  toot text failed. device: %s toot id: %llu",
    //                     toot->device->name, toot->id);
    toot->state = TOOT_ERROR;
    return false;
  }
  // cremona_toot_lock(toot);
  toot->state = WRITE_COMPLEATE;
  toot->prev_count = toot->send_count;
  toot->send_count = 0;
  // notify(toot);
  // cremona_toot_unlock(toot);
  return true;
}
static bool open_wait_cond(void *context) {
  cremona_toot_t *toot = (cremona_toot_t *)context;
  return toot->state == OPEND || toot->state == DESTROYED ||
         toot->state == TOOT_ERROR;
}

static bool write_wait_cond(void *context) {
  cremona_toot_t *toot = (cremona_toot_t *)context;
  return toot->state == WRITE_COMPLEATE || toot->state == DESTROYED ||
         toot->state == TOOT_ERROR;
}

static bool close_wait_cond(void *context) {
  cremona_toot_t *toot = (cremona_toot_t *)context;
  return toot->state == DESTROYED || toot->state == TOOT_ERROR;
}

void wait_toot(cremona_toot_t *toot, toot_wait_type_t wait_type) {
  switch (wait_type) {
  case WAIT_OPEN:
    waiter_wait(&toot->wait, &open_wait_cond, toot, 1000, NULL);
    break;
  case WAIT_CLOSE:
    waiter_wait(&toot->wait, &close_wait_cond, toot, 1000, NULL);
    break;
  case WAIT_WRITE:
    waiter_wait(&toot->wait, &write_wait_cond, toot, 1000, NULL);
    break;
  default:
    break;
  }
}
