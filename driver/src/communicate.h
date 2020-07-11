#ifndef CRMNA_COMMUNICATE_HEADER
#define CRMNA_COMMUNICATE_HEADER

#include "common.h"
#include "message.h"

#include "interfaces/commuinicator.h"
#include "interfaces/communicator_factory.h"
#include "interfaces/id_mapper.h"
#include "interfaces/id_mapper_factory.h"
#include "interfaces/locker.h"
#include "interfaces/locker_factory.h"
#include "interfaces/logger.h"

typedef struct communicate {
  communicator_ref communicator;
  id_mapper_ref new_toot_messages;
  id_mapper_ref add_toot_messages;
  id_mapper_ref send_toot_messages;
  locker_ref lock;
  logger *logger_ref;
  cremona_device_manager_t *device_manager;
} communicate_t;

void clear_communicate(communicate_t *com);
bool init_communicate(communicate_t *com,
                      communicator_factory_ref *communicator_factory,
                      locker_factory_ref *locker_factory,
                      id_mapper_factory_ref *id_mapper_factory,
                      cremona_device_manager_t *device_manager,
                      logger *logger_ref, crmna_err *err);

void destory_communicate(communicate_t *com);

bool send_new_toot(communicate_t *com, int pid, new_toot_t *message,
                   cremona_toot_t *toot, crmna_err *err);
void cancel_wait_new_toot_result(communicate_t *com, new_toot_t *message);
bool reciveMessage(communicate_t *com, uint32_t pid, int type, crmna_buf_t *buf,
                   crmna_err *error);

#endif