#include "locker.h"

void locker_lock(locker_ref *ref) { ref->interface->lock(ref->obj); }

void locker_unlock(locker_ref *ref) { ref->interface->unlock(ref->obj); }

void locker_free(locker_ref *ref) {
  if (ref == NULL) {
    return;
  }
  if (ref->interface != NULL) {
    ref->interface->free(ref->obj);
  }
  ref->interface = NULL;
  ref->obj = NULL;
}

void clear_locker_ref(locker_ref *ref) {
  ref->interface = NULL;
  ref->obj = NULL;
}
