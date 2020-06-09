#include "cremona_internal.h"

bool add_device(cremona_device_manager_t *device_manager,
                cremona_device_t *device) {
  int ret;

  cremona_device_manager_lock(device_manager);

  cremona_device_unlock(device);

  cremona_device_manager_unlock(device_manager);
  return true;
}

cremona_device_t *cremna_get_device(cremona_device_manager_t *device_manager,
                                    uint64_t id) {
  cremona_device_manager_lock(device_manager);

  cremona_device_t *device = NULL;

  cremona_device_manager_unlock(device_manager);

  return add_ref_device(device);
}

void delete_device(cremona_device_manager_t *device_manager, uint64_t id) {
  cremona_device_manager_lock(device_manager);



  cremona_device_manager_unlock(device_manager);

  //release_device(device);
}
