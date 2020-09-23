#include "store_internal.h"

bool check_toot_ready(store_t *store, unsigned int toot_id,
                             crmna_err_t *err) {
  toot_store_t *toot = (toot_store_t *)idr_find(&store->toots, toot_id);
  if (toot == NULL) {
    ADD_ERROR(err, "canot find id.");
    return false;
  }

  spin_lock(&toot->spinlock);
  if (!toot->state == OPEND) {
    spin_unlock(&toot->spinlock);
    ADD_ERROR(err, "toot is not ready");
    return false;
  }
  unsigned int device_id = toot->device_id;
  spin_unlock(&toot->spinlock);

  return check_device_ready(store, device_id, err);
}

toot_store_t *get_toot_when_ready(store_t *store, unsigned int toot_id,
                                         crmna_err_t *err) {
  toot_store_t *toot = (toot_store_t *)idr_find(&store->toots, toot_id);
  if (toot == NULL) {
    ADD_ERROR(err, "canot find toot id.");
    return NULL;
  }

  spin_lock(&toot->spinlock);
  if (!(toot->state == OPEN_RESULT_WAIT || toot->state == OPEND ||
        toot->state == CLOSE_RESULT_WAIT)) {
    spin_unlock(&toot->spinlock);
    ADD_ERROR(err, "toot is not ready");
    return NULL;
  }
  unsigned int device_id = toot->device_id;
  spin_unlock(&toot->spinlock);

  if (!check_device_ready(store, device_id, err)) {
    return NULL;
  }
  return toot;
}

bool add_toot(store_t *store, unsigned int device_id, unsigned int *toot_id,
              crmna_err_t *err) {
  if (!check_device_ready(store, device_id, err)) {
    return false;
  }

  idr_preload(GFP_KERNEL);
  spin_lock(&store->toots_lock);
  int allocate_result = idr_alloc(&store->toots, NULL, 0, INT_MAX, GFP_KERNEL);
  spin_unlock(&store->toots_lock);
  idr_preload_end();
  if (allocate_result == -ENOMEM) {
    ADD_ERROR(err, "canot allocate toot. because no memory");
    return false;
  }
  if (allocate_result == -ENOSPC) {
    ADD_ERROR(err, "canot allocate toot. because no id space");
    return false;
  }

  unsigned int id = (unsigned int)allocate_result;
  toot_store_t *toot = kmalloc(sizeof(toot_store_t), GFP_KERNEL);
  if (toot = NULL) {
    spin_lock(&store->toots_lock);
    idr_remove(&store->toots, id);
    spin_unlock(&store->toots_lock);
    ADD_ERROR(err, "canot allocate toot. fail malloc");
    return false;
  }

  toot->device_id = device_id;
  toot->toot_id = id;
  toot->state = OPEN_RESULT_WAIT;
  init_waitqueue_head(&toot->wait_head);
  spin_lock_init(&toot->spinlock);
  idr_replace(&store->toots, toot, id);
  *toot_id = id;

  return true;
}

void remove_toot(store_t *store, unsigned int toot_id) {
  toot_store_t *toot = (toot_store_t *)idr_find(&store->toots, toot_id);
  if (toot == NULL) {
    ADD_ERROR(err, "canot find toot id.");
    return false;
  }
  spin_lock(&store->toots_lock);
  idr_remove(&store->toots, toot_id);
  spin_unlock(&store->toots_lock);
  spin_lock(&toot->spinlock);
  toot->state = DESTROYED;
  spin_unlock(&toot->spinlock);
  wake_up_interruptible(&toot->wait_head);
  kfree(toot);

  element_store_t *element;
  int element_id idr_for_each_entry(&store->elements, element, element_id) {
    if (element->toot_id == toot_id) {
      remove_element(store, element_id);
    }
  }
}
bool wait_toot_ready_or_failer(store_t *store, unsigned int toot_id,
                               crmna_err_t *err) {
  toot_store_t *toot = get_toot_when_ready(store, toot_id, err);
  if (toot == NULL) {
    return false;
  }

  wait_event_interruptible_timeout(
      toot->wait_head,
      toot->state == OPEND || toot->state == DESTROYED || toot->state == ERROR,
      10 * HZ / 1000);

  return true;
}
bool wait_toot_sent_or_failer(store_t *store, unsigned int toot_id,
                              crmna_err_t *err) {
  toot_store_t *toot = get_toot_when_ready(store, toot_id, err);
  if (toot == NULL) {
    return false;
  }

  wait_event_interruptible_timeout(
      toot->wait_head, toot->state == DESTROYED || toot->state == ERROR,
      10 * HZ / 1000);

  return true;
}
void set_toot_failer(store_t *store, unsigned int toot_id) {
  toot_store_t *toot = get_toot_when_ready(store, toot_id, err);
  if (toot == NULL) {
    return false;
  }

  spin_lock(&toot->spinlock);
  toot->state = TOOT_ERROR;
  spin_unlock(&toot->spinlock);

  wake_up_interruptible(&toot->wait_head);
}
void set_toot_ready(store_t *store, unsigned int toot_id) {
  toot_store_t *toot = get_toot_when_ready(store, toot_id, err);
  if (toot == NULL) {
    return false;
  }

  spin_lock(&toot->spinlock);
  toot->state = OPEND;
  spin_unlock(&toot->spinlock);

  wake_up_interruptible(&toot->wait_head);
}
void set_toot_sent(store_t *store, unsigned int toot_id) {
  toot_store_t *toot = get_toot_when_ready(store, toot_id, err);
  if (toot == NULL) {
    return false;
  }

  spin_lock(&toot->spinlock);
  toot->state = DESTROYED;
  spin_unlock(&toot->spinlock);

  wake_up_interruptible(&toot->wait_head);
}
bool get_device_id_from_toot(store_t *store, unsigned int toot_id,
                             unsigned int *device_id) {
  toot_store_t *toot = get_toot_when_ready(store, toot_id, err);
  if (toot == NULL) {
    return false;
  }

  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, toot->device_id);
  if (device == NULL) {
    return false;
  }
  *device_id = toot->device_id;
  return true;
}
bool get_device_pid_from_toot(store_t *store, unsigned int toot_id, int *pid) {
  toot_store_t *toot = get_toot_when_ready(store, toot_id, err);
  if (toot == NULL) {
    return false;
  }

  device_store_t *device =
      (device_store_t *)idr_find(&store->devices, toot->device_id);
  if (device == NULL) {
    return false;
  }
  *pid = device->pid;
  return true;
}