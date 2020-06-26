#include "locker.h"

bool locker_lock(locker_ref *ref, crmna_err_t *err) {
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
  if (ref->interface->lock == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"lock\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->lock(ref->obj, err);
}

bool locker_unlock(locker_ref *ref, crmna_err_t *err) {
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
  if (ref->interface->unlock == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"unlock\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->unlock(ref->obj, err);
}

bool locker_free(locker_ref *ref, crmna_err_t *err) {
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

void clear_locker_ref(locker_ref *ref) {
  ref->interface = NULL;
  ref->obj = NULL;
}
