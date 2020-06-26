#include "waiter_factory.h"

bool create_waiter(waiter_factory_ref *factory, waiter_ref *ref,
                   crmna_err_t *err) {
  if (factory == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (factory->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (factory->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (factory->interface->create_waiter == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"create_waiter\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "output target reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  ref->interface = NULL;
  ref->obj = NULL;

  return factory->interface->create_waiter(factory->obj, ref, err);
}