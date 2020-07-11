#include "cremona_internal.h"

void clear_communicate(communicate_t *com) {
  clear_communicator_ref(&com->communicator);
  clear_id_mapper_ref(&com->new_toot_messages);
  clear_id_mapper_ref(&com->add_toot_messages);
  clear_id_mapper_ref(&com->send_toot_messages);
  clear_locker_ref(&com->lock);
}

bool init_communicate(communicate_t *com,
                      communicator_factory_ref *communicator_factory,
                      locker_factory_ref *locker_factory,
                      id_mapper_factory_ref *id_mapper_factory,
                      cremona_device_manager_t *device_manager,
                      logger *logger_ref, crmna_err *err) {
  com->logger_ref = logger_ref;
  com->device_manager = device_manager;

  if (!create_locker(locker_factory, &com->lock, err)) {
    LOG_AND_ADD_ERROR(logger_ref, err,
                      "Cannot allocate locker in communicate.");
    destory_communicate(com);
    return false;
  }
  if (!create_communicator(communicator_factory, com, &com->communicator,
                           err)) {
    LOG_AND_ADD_ERROR(logger_ref, err,
                      "Cannot allocate communicator in communicate.");
    destory_communicate(com);
    return false;
  }
  if (!create_id_mapper(id_mapper_factory, &com->new_toot_messages, err)) {
    LOG_AND_ADD_ERROR(logger_ref, err,
                      "Cannot allocate new_toot_messages in communicate.");
    destory_communicate(com);
    return false;
  }
  if (!create_id_mapper(id_mapper_factory, &com->add_toot_messages, err)) {
    LOG_AND_ADD_ERROR(logger_ref, err,
                      "Cannot allocate add_toot_messages in communicate.");
    destory_communicate(com);
    return false;
  }
  if (!create_id_mapper(id_mapper_factory, &com->send_toot_messages, err)) {
    LOG_AND_ADD_ERROR(logger_ref, err,
                      "Cannot allocate send_toot_messages in communicate.");
    destory_communicate(com);
    return false;
  }

  return true;
}
void destory_communicate(communicate_t *com) {
  id_mapper_free(&com->new_toot_messages);
  id_mapper_free(&com->add_toot_messages);
  id_mapper_free(&com->send_toot_messages);
  communicator_free(&com->communicator);
}

bool send_new_toot(communicate_t *com, int pid, new_toot_t *message,
                   cremona_toot_t *toot, crmna_err *err) {

  int message_id = -1;
  if (!id_mapper_add_get_id(&com->new_toot_messages, NULL, &message_id, err)) {
    LOG_AND_ADD_ERROR(com->logger_ref, err,
                      "Fail assign message id to new_toot message." JSON_OUT(
                          pid, d, miner, d, toot_id, d),
                      pid, toot->miner, toot->id);
    return false;
  }

  char buf[100];
  size_t msg_size = serialize_new_toot(message, buf, sizeof(buf));
  crmna_buf_t send_buf = {.buf = buf, .buf_size = msg_size};
  if (communicator_send_message(&com->communicator, pid,
                                CRMNA_CREATE_DEVICE_RESULT, &send_buf,
                                err) == -1) {
    id_mapper_remove(&com->new_toot_messages, message_id, err);
    LOG_AND_ADD_ERROR(com->logger_ref, err,
                      "Cannot send new toot message." JSON_OUT(
                          pid, d, miner, d, toot_id, d),
                      pid, toot->miner, toot->id);
    return false;
  }

  if (!id_mapper_replace(&com->new_toot_messages, message_id,add_ref_toot(toot), err)) {
    release_toot(toot);
    id_mapper_remove(&com->new_toot_messages, message_id, err);
    LOG_AND_ADD_ERROR(com->logger_ref, err,
                      "Fail assign toot to new_toot wait list." JSON_OUT(
                          pid, d, miner, d, toot_id, d),
                      pid, toot->miner, toot->id);
    return false;
  }
  return true;
}

void cancel_wait_new_toot_result(communicate_t *com, new_toot_t *message){
  id_mapper_remove(&com->new_toot_messages, message->message_id, NULL);
}

static bool get_stats(communicate_t *com, uint32_t pid) {}
static bool create_device_process(communicate_t *com, uint32_t pid,
                                  crmna_buf_t *buf, crmna_err *error) {
  create_device_t recive;
  if (!deserialize_create_conn(buf->buf, buf->buf_size, &recive)) {
    LOG_AND_ADD_ERROR(com->logger_ref, error,
                      "Cannot decode create device message. pid: %d", pid);
    return false;
  }
  create_device_result_t result;
  if (!create_device_message(com->device_manager, &recive, pid, &result,
                             error)) {
    return false;
  }
  char message[100];
  int msg_size =
      serialize_create_device_result(&result, message, sizeof(message));
  crmna_buf_t send_buf = {.buf = message, .buf_size = msg_size};
  if (communicator_send_message(&com->communicator, pid,
                                CRMNA_CREATE_DEVICE_RESULT, &send_buf,
                                error) == -1) {
    LOG_AND_ADD_ERROR(com->logger_ref, error,
                      "Cannot send init device message. pid: %d", pid);
    return false;
  }
  return true;
}
static bool destroy_device_process(communicate_t *com, uint32_t pid,
                                   crmna_buf_t *buf, crmna_err *error) {}
static bool new_toot_result_process(communicate_t *com, uint32_t pid,
                                    crmna_buf_t *buf, crmna_err *error) {
  new_toot_result_t recive;
  if (!deserialize_new_toot_result(buf->buf, buf->buf_size, &recive)) {
    LOG_AND_ADD_ERROR(com->logger_ref, error,
                      "Cannot decode new toot message. " JSON_OUT(pid, d), pid);
    return false;
  }

  cremona_toot_t *toot = (cremona_toot_t *)id_mapper_find(
      &com->new_toot_messages, recive.message_id, error);
  if (toot == NULL) {
    LOG_AND_ADD_ERROR(
        com->logger_ref, error,
        "Cannot find toot that waiting new toot message. " JSON_OUT(
            pid, d, message_id, d),
        pid, recive.message_id);
    return false;
  }

  if (toot->miner != recive.device_id || toot->id != recive.toot_id) {
    LOG_AND_ADD_ERROR(com->logger_ref, error,
                      "Fail match new toot message and toot. " JSON_OUT(
                          pid, d, message_id, d, message_miner, d,
                          message_toot_id, d, miner, d, toot_id, d),
                      pid, recive.message_id, recive.device_id, recive.toot_id,
                      toot->miner, toot->id);
    return false;
  }

  if (!recive_open_toot_result(toot, &recive, error)) {
    LOG_AND_ADD_ERROR(com->logger_ref, error,
                      "Fail process new toot message in toot. " JSON_OUT(
                          pid, d, message_id, d, miner, d, toot_id, d),
                      pid, recive.message_id, toot->miner, toot->id);
    return false;
  }

  id_mapper_remove(&com->new_toot_messages, recive.message_id, error);
  release_toot(toot);
  return true;
}
static bool send_toot_result_process(communicate_t *com, uint32_t pid,
                                     crmna_buf_t *buf, crmna_err *error) {}
static bool add_toot_text_result_process(communicate_t *com, uint32_t pid,
                                         crmna_buf_t *buf, crmna_err *error) {}

bool reciveMessage(communicate_t *com, uint32_t pid, int type, crmna_buf_t *buf,
                   crmna_err *error) {
  switch (type) {
  case CRMNA_GET_STATS:
    return get_stats(com, pid);
  case CRMNA_CREATE_DEVICE:
    return create_device_process(com, pid, buf, error);
  case CRMNA_DESTROY_DEVICE:
    return destroy_device_process(com, pid, buf, error);
  case CRMNA_NEW_TOOT_RESULT:
    return new_toot_result_process(com, pid, buf, error);
  case CRMNA_SEND_TOOT_RESULT:
    return send_toot_result_process(com, pid, buf, error);
  case CRMNA_ADD_TOOT_TEXT_RESULT:
    return add_toot_text_result_process(com, pid, buf, error);

  default:
    LOG_AND_ADD_ERROR(com->logger_ref, error,
                      "Unknown message type. type: %d pid: %u", type, pid);
    return false;
  }
}
