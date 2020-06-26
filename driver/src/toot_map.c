#include "cremona_internal.h"
bool add_toot(cremona_device_t *device, cremona_toot_t *toot) {
  int ret;
  // cremona_device_lock(device);

  // k = kh_put(toot, device->toots, toot->id, &ret);

  // kh_value(device->toots, k) = add_ref_toot(toot);

  //  cremona_device_unlock(device);
  return true;
}

cremona_toot_t *cremna_get_toot(cremona_device_t *device, uint64_t id) {
  // cremona_device_lock(device);

  cremona_toot_t *toot = NULL;
  // if (k != kh_end(device->toots)) {
  //   toot = kh_val(device->toots, k);
  // }

  // cremona_device_unlock(device);

  return add_ref_toot(toot);
}

void delete_toot(cremona_device_t *device, uint64_t id) {
  // cremona_device_lock(device);

  // cremona_device_unlock(device);

  // release_toot(toot);
}

int count_toot(cremona_device_t *device) {
  //    cremona_device_lock(device);

  //  cremona_device_unlock(device);
  return 0;
}
