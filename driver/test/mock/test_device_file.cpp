#include "test_device_file.h"

bool test_device_file::free(void *obj, crmna_err_t *err) {
  return ((test_device_file *)obj)->free(err);
}

device_file test_device_file::interface = {.free = &test_device_file::free};

device_file_ref test_device_file::get_ref() {
  return {.interface = &test_device_file::interface, .obj = this};
}

bool test_device_file_factory::create_device_file(void *obj,
                                                  cremona_device_t *device,
                                                  device_file_ref *device_file,
                                                  crmna_err_t *err) {
  return ((test_device_file_factory *)obj)
      ->create_device_file(device, device_file, err);
}

device_file_factory test_device_file_factory::interface = {
    .create_device_file = &test_device_file_factory::create_device_file};

device_file_factory_ref test_device_file_factory::get_factory() {
  return {.interface = &test_device_file_factory::interface, .obj = this};
}