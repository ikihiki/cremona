#include "waiter.h"

bool waiter_wait(waiter_ref *ref, cond_func cond, void *context, int msec,
                 crmna_err_t *err) {
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->wait == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"wait\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->wait(ref->obj,cond, context, msec, err);
}

bool waiter_notify(waiter_ref *ref, crmna_err_t *err) {
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->notify == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"notify\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->notify(ref->obj, err);
}

bool waiter_free(waiter_ref *ref, crmna_err_t *err) {
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->free == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"free\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->free(ref->obj, err);
}
void clear_waiter_ref(waiter_ref *ref) {
  ref->interface = NULL;
  ref->obj = NULL;
}
