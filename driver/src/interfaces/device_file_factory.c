#include "device_file_factory.h"

bool create_device_file(device_file_factory_ref *factory,
                        cremona_device_t *device, device_file_ref *ref,
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
  if (factory->interface->create_device_file == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
             "function pointer \"create_device_file\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
             "output target reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (device == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
             "device is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  ref->interface = NULL;
  ref->obj = NULL;

  return factory->interface->create_device_file(factory->obj, device,ref, err);
}