#include "cremona_internal.h"
bool add_toot(cremona_device_t *device, cremona_toot_t *toot) {
  int ret;
  khiter_t k;
  cremona_device_lock(device);

  k = kh_put(toot, device->toots, toot->id, &ret);

  kh_value(device->toots, k) = add_ref_toot(toot);

  cremona_device_unlock(device);
  return true;
}

cremona_toot_t *cremna_get_toot(cremona_device_t *device, uint64_t id) {
  cremona_device_lock(device);

  khint_t k = kh_get(toot, device->toots, id);
  cremona_toot_t *toot = NULL;
  if (k != kh_end(device->toots)) {
    toot = kh_val(device->toots, k);
  }

  cremona_device_unlock(device);

  return add_ref_toot(toot);
}

void delete_toot(cremona_device_t *device, uint64_t id){
  cremona_device_lock(device);

  khint_t k = kh_get(toot, device->toots, id);
  if (k == kh_end(device->toots)) {
    cremona_device_unlock(device);
    return;
  }
  cremona_toot_t *toot = kh_val(device->toots, k);
  kh_del(toot, device->toots, k);

  cremona_device_unlock(device);

  release_toot(toot);
}

int count_toot(cremona_device_t *device){
    khint_t k;
    int count = 0;
    cremona_device_lock(device);

    for (k = kh_begin(device->toots); k != kh_end(device->toots); ++k)
    {
        if (kh_exist(device->toots, k))
        {
            count++;
        }
  }

  cremona_device_unlock(device);
  return count;
}
