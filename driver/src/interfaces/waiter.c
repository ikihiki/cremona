#include "waiter.h"

bool waiter_wait(waiter_ref_t *ref, crmna_err_t *err) {
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
  if (ref->interface->wait == NULL) {
    err->error_msg = "function pointer \"wait\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->wait(ref->obj, err);
}

bool waiter_notify(waiter_ref_t *ref, crmna_err_t *err) {
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
  if (ref->interface->notify == NULL) {
    err->error_msg = "function pointer \"notify\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->notify(ref->obj, err);
}

bool waiter_free(waiter_ref_t *ref, crmna_err_t *err) {
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

bool create_waiter(waiter_factory_t *factory, waiter_ref_t *ref,
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

  return factory->create_waiter(ref, err);
}