#include "device_file_factory.h"

bool create_device_file(device_file_factory_ref *factory,
                        cremona_device_t *device, device_file_ref *ref,
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
  if (factory->interface->create_device_file == NULL) {
    ADD_ERROR(err, "function pointer \"create_device_file\" is null");
    return false;
  }
  if (ref == NULL) {
    ADD_ERROR(err, "output target reference is null");

    return false;
  }
  if (device == NULL) {
    ADD_ERROR(err, "device is null");
    return false;
  }
  ref->interface = NULL;
  ref->obj = NULL;

  return factory->interface->create_device_file(factory->obj, device, ref, err);
}