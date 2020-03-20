#include "cremona_internal.h"

bool add_device(cremona_device_manager_t *device_manager,
                cremona_device_t *device) {
  int ret;
  khiter_t k;

  cremona_device_manager_lock(device_manager);

  k = kh_put(dm, device_manager->devices, device->id, &ret);

  cremona_device_lock(device);
  kh_value(device_manager->devices, k) = add_ref_device(device);
  cremona_device_unlock(device);

  cremona_device_manager_unlock(device_manager);
  return true;
}

cremona_device_t *cremna_get_device(cremona_device_manager_t *device_manager,
                                    uint64_t id) {
  cremona_device_manager_lock(device_manager);

  khint_t k = kh_get(dm, device_manager->devices, id);
  cremona_device_t *device = NULL;
  if (k != kh_end(device_manager->devices)) {
    device = kh_val(device_manager->devices, k);
  }

  cremona_device_manager_unlock(device_manager);

  return add_ref_device(device);
}

void delete_device(cremona_device_manager_t *device_manager, uint64_t id) {
  cremona_device_manager_lock(device_manager);

  khint_t k = kh_get(dm, device_manager->devices, id);
  if (k == kh_end(device_manager->devices)) {
    cremona_device_manager_unlock(device_manager);
    return;
  }
  cremona_device_t *device = kh_val(device_manager->devices, k);
  kh_del(dm, device_manager->devices, k);

  cremona_device_manager_unlock(device_manager);

  release_device(device);
}
