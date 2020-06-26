#include "communicator_factory.h"

bool create_communicator(communicator_factory_ref *factory,
                         cremona_device_manager_t *device_manager,
                         communicator_ref *ref, crmna_err_t *err) {
  if (factory == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "factory is null");
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
  if (factory->interface->create_communicator == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
             "function pointer \"create_communicator\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
             "output target reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (device_manager == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "device manager is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  ref->interface = NULL;
  ref->obj = NULL;

  return factory->interface->create_communicator(factory->obj, device_manager,
                                                 ref, err);
}