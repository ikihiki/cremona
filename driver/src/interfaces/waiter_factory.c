#include "waiter_factory.h"

bool create_waiter(waiter_factory_ref *factory, waiter_ref *ref,
                   crmna_err *err) {
  if (factory == NULL) {
    ADD_ERROR(err, "reference is null");
    return false;
  }
  if (factory->interface == NULL) {
    ADD_ERROR(err, "interface is null");
    return false;
  }
  if (factory->obj == NULL) {
    ADD_ERROR(err, "interface reference is null");
    return false;
  }
  if (factory->interface->create_waiter == NULL) {
    ADD_ERROR(err, "function pointer \"create_waiter\" is null");
    return false;
  }
  if (ref == NULL) {
    ADD_ERROR(err, "output target reference is null");
    return false;
  }
  ref->interface = NULL;
  ref->obj = NULL;

  return factory->interface->create_waiter(factory->obj, ref, err);
}