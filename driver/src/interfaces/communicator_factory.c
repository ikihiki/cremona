#include "communicator_factory.h"

bool create_communicator(communicator_factory_ref *factory,
                         communicate_t *communicate, communicator_ref *ref,
                         crmna_err *err) {
  if (factory == NULL) {
    ADD_ERROR(err, "factory is null");
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
  if (factory->interface->create_communicator == NULL) {
    ADD_ERROR(err, "function pointer \"create_communicator\" is null");
    return false;
  }
  if (ref == NULL) {
    ADD_ERROR(err, "output target reference is null");
    return false;
  }
  if (communicate == NULL) {
    ADD_ERROR(err, "device manager is null");
    return false;
  }
  ref->interface = NULL;
  ref->obj = NULL;

  return factory->interface->create_communicator(factory->obj, communicate, ref,
                                                 err);
}