#ifndef __CREMONA_HEADER__
#define __CREMONA_HEADER__

#include "common.h"
#include "message.h"
#include "communicate.h"
#include "interfaces/allocator.h"
#include "interfaces/commuinicator.h"
#include "interfaces/communicator_factory.h"
#include "interfaces/device_file_factory.h"
#include "interfaces/id_mapper.h"
#include "interfaces/id_mapper_factory.h"
#include "interfaces/locker.h"
#include "interfaces/locker_factory.h"
#include "interfaces/logger.h"
#include "interfaces/waiter.h"
#include "interfaces/waiter_factory.h"

typedef enum toot_state {
  CREANUPED,
  OPEN_RESULT_WAIT,
  OPEND,
  ADD_TEXT_RESULT_WAIT,
  WRITE_COMPLEATE,
  CLOSE_RESULT_WAIT,
  DESTROYED,
  TOOT_ERROR
} toot_state_t;

typedef enum toot_wait_type {
  WAIT_OPEN,
  WAIT_WRITE,
  WAIT_CLOSE
} toot_wait_type_t;

typedef struct cremona_toot {
  int id;
  int miner;
  int pid;
  toot_state_t state;
  // char buf[500];
  int prev_count;
  int send_count;
  int refCount;
  allocator_ref *alloc;
  logger *logger_ref;
  locker_ref lock;
  waiter_ref wait;
  communicate_t *comm;
} cremona_toot_t;

typedef struct cremona_toot_callbacks {
  void (*lock)(cremona_toot_t *toot);
  void (*unlock)(cremona_toot_t *toot);
  void (*wait)(cremona_toot_t *toot, toot_wait_type_t wait_type);
  void (*notify)(cremona_toot_t *toot);
  cremona_toot_t *(*create_toot)(void);
  bool (*destroy_toot)(cremona_toot_t *toot);
  void (*cleanup_toot)(cremona_toot_t *toot);
} cremona_toot_callbacks_t;

typedef struct cremona_device {
  char name[50];
  int miner;
  int pid;
  int uid;
  bool isDestroied;
  int refCount;
  id_mapper_ref toots;
  locker_ref lock;
  device_file_ref device_file;
  communicate_t *comm;
  locker_factory_ref *locker_factory;
  id_mapper_factory_ref *id_mapper_factory;
  waiter_factory_ref *waiter_factory;

  logger *logger_ref;
  allocator_ref *alloc;
} cremona_device_t;

typedef struct cremona_device_manager cremona_device_manager_t;


typedef struct cremona_device_manager {
  int refCount;
  id_mapper_ref devices;
  locker_ref lock;
  communicate_t comm;
  locker_factory_ref *locker_factory;
  id_mapper_factory_ref *id_mapper_factory;
  waiter_factory_ref *waiter_factory;
  device_file_factory_ref *device_file_factory;
  logger *logger_ref;
  allocator_ref *alloc;
  int driver_number_min;
  int driver_number_max;
} cremona_device_manager_t;

bool init_device_manager(cremona_device_manager_t *device_manager,
                         int driver_number_min, int driver_number_max,
                         communicator_factory_ref *communicator_factory,
                         locker_factory_ref *locker_factory,
                         id_mapper_factory_ref *id_mapper_factory,
                         waiter_factory_ref *waiter_factory,
                         device_file_factory_ref *device_file_factory,
                         logger *logger, allocator_ref *alloc, crmna_err *err);
bool destroy_device_manager(cremona_device_manager_t *device_manager,
                            crmna_err *err);

void *cremona_malloc(cremona_device_manager_t *device_manager, int size);

cremona_device_t *get_cremona_device(cremona_device_manager_t *device_manager,
                                     uint64_t id);

cremona_toot_t *add_ref_toot(cremona_toot_t *toot);
void release_toot(cremona_toot_t *toot);
cremona_toot_t *open_toot(cremona_device_t *device, bool wait, crmna_err *err);
bool close_toot(cremona_toot_t *toot, crmna_err *err);

bool add_toot_text(cremona_toot_t *toot, char *text, bool wait, crmna_err *err);

#endif