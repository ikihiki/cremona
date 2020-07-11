#include "waiter.h"

bool waiter_wait(waiter_ref *ref, cond_func cond, void *context, int msec,
                 crmna_err *err) {
  if (ref == NULL) {
    ADD_ERROR(err, "reference is null");

    return false;
  }
  if (ref->interface == NULL) {
    ADD_ERROR(err, "interface is null");

    return false;
  }
  if (ref->obj == NULL) {
    ADD_ERROR(err, "interface reference is null");

    return false;
  }
  if (ref->interface->wait == NULL) {
    ADD_ERROR(err, "function pointer \"wait\" is null");

    return false;
  }
  return ref->interface->wait(ref->obj, cond, context, msec, err);
}

void waiter_notify(waiter_ref *ref) {
  return ref->interface->notify(ref->obj);
}

void waiter_free(waiter_ref *ref) {
  if (ref == NULL) { 
    return; 
  }
  if (ref->interface != NULL) {
    ref->interface->free(ref->obj);
  }
  ref->interface = NULL;
  ref->obj = NULL;
}

void clear_waiter_ref(waiter_ref *ref) {
  ref->interface = NULL;
  ref->obj = NULL;
}
