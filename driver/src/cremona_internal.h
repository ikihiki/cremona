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
void release_miner_num(cremona_device_manager_t *device_manager, unsigned int miner);

// util
uint64_t create_id(cremona_device_manager_t *device_manager);

// device
cremona_device_t *add_ref_device(cremona_device_t *device);
bool init_device(
    cremona_device_t *device, int miner,
    uint32_t pid, uint32_t uid, char *name,
    id_mapper_factory_ref *id_mapper_factory,
    locker_factory_ref *locker_factory, communicator_ref *comm,
    waiter_factory_ref *waiter_factory,device_file_factory_ref *device_file_factory, logger *logger_ref, allocator_ref *alloc,
    crmna_err_t *error);

void cremona_device_lock(cremona_device_t *device);
//void cremona_device_unlock(cremona_device_t *device);
cremona_device_t *get_cremona_device(cremona_device_manager_t *device_manager,
                                     uint64_t id);
bool destroy_device(cremona_device_t *device, crmna_err_t *error);
void release_device(cremona_device_t *device);
bool recive_close_toot_result(cremona_toot_t *toot, cremona_device_t *device,
                              send_toot_result_t *message, crmna_err_t *error);

// toot map
bool add_toot(cremona_device_t *device, cremona_toot_t *toot);
cremona_toot_t *cremna_get_toot(cremona_device_t *device, uint64_t id);
void delete_toot(cremona_device_t *device, uint64_t id);
int count_toot(cremona_device_t *device);

// toot
cremona_toot_t *create_toot(cremona_device_t *device, crmna_err_t *error);
bool recive_open_toot_result(cremona_toot_t *toot, new_toot_result_t *message,
                             crmna_err_t *err);
void cremona_toot_lock(cremona_toot_t *toot);
void cremona_toot_unlock(cremona_toot_t *toot);
bool destroy_toot(cremona_toot_t *toot, crmna_err_t *error);
void wait_toot(cremona_toot_t *toot, toot_wait_type_t wait_type);
bool recive_add_toot_text_result(cremona_toot_t *toot,
                                 add_toot_text_result_t *message,
                                 crmna_err_t *err);

#endif