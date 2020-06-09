#include "locker.h"

bool locker_lock(locker_ref_t *ref, crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->lock == NULL) {
    err->error_msg = "function pointer \"lock\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->lock(ref->obj, err);
}

bool locker_unlock(locker_ref_t *ref, crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->unlock == NULL) {
    err->error_msg = "function pointer \"unlock\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->unlock(ref->obj, err);
}

bool locker_free(locker_ref_t *ref, crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->free == NULL) {
    err->error_msg = "function pointer \"free\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->free(ref->obj, err);
}

bool create_locker(locker_factory_t *factory, locker_ref_t *ref,
                          crmna_err_t *err) {
  if (factory == NULL) {
    err->error_msg = "factory is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref == NULL) {
    err->error_msg = "output target reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  ref->interface = NULL;
  ref->obj = NULL;

  return factory->create_locker(ref, err);
}