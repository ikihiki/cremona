#ifndef __CREMONA_INTERNAL__
#define __CREMONA_INTERNAL__

#include "cremona.h"
#include "message.h"

// device_manager
void cremona_device_manager_lock(cremona_device_manager_t *device_manager);
void cremona_device_manager_unlock(cremona_device_manager_t *device_manager);
cremona_device_manager_t *
add_ref_device_manager(cremona_device_manager_t *device_manager);
void release_device_manager(cremona_device_manager_t *device_manager);
int send_message(cremona_device_manager_t *device_manager, uint32_t pid,
                 int type, char *buf, size_t buf_size);
bool rent_miner_num(cremona_device_manager_t *device_manager,
                    unsigned int *miner);
void release_miner_num(cremona_device_manager_t *device_manager,
                       unsigned int miner);

bool create_device_message(cremona_device_manager_t *device_manager,
                           create_device_t *recive_message, int pid,
                           create_device_result_t *result_message,
                           crmna_err *error);

bool destroy_device_message(cremona_device_manager_t *device_manager,
                            destroy_device_t *recive_message, int pid,
                            destroy_device_result_t *result_message,
                            crmna_err *error);

// device
cremona_device_t *add_ref_device(cremona_device_t *device);
cremona_device_t *
create_device(int miner, uint32_t pid, uint32_t uid, char *name,
              id_mapper_factory_ref *id_mapper_factory,
              locker_factory_ref *locker_factory, communicate_t *comm,
              waiter_factory_ref *waiter_factory,
              device_file_factory_ref *device_file_factory, logger *logger_ref,
              allocator_ref *alloc, crmna_err *error);

cremona_device_t *get_cremona_device(cremona_device_manager_t *device_manager,
                                     uint64_t id);
void destroy_device(cremona_device_t *device);
void release_device(cremona_device_t *device);
bool recive_close_toot_result(cremona_toot_t *toot, cremona_device_t *device,
                              send_toot_result_t *message, crmna_err *error);

// toot
cremona_toot_t *create_toot(int id, int miner, int pid, allocator_ref *alloc,
                            locker_factory_ref *locker_factory,
                            waiter_factory_ref *waiter_factory,
                            communicate_t *comm, bool wait, logger *logger_ref,
                            crmna_err *error);
bool recive_open_toot_result(cremona_toot_t *toot, new_toot_result_t *message,
                             crmna_err *err);
void cremona_toot_lock(cremona_toot_t *toot);
void cremona_toot_unlock(cremona_toot_t *toot);
void wait_toot(cremona_toot_t *toot, toot_wait_type_t wait_type);
bool recive_add_toot_text_result(cremona_toot_t *toot,
                                 add_toot_text_result_t *message,
                                 crmna_err *err);



#endif