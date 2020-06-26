#include <iostream>
#include <string>
#include "test_communicator.h"


int test_communicator::send_message(void *obj, uint32_t pid, int type,
                                    crmna_buf_t *buf, crmna_err_t *err) {
  return ((test_communicator *)obj)->send_message(pid, type, buf, err);
}
bool test_communicator::free(void *obj, crmna_err_t *err) {
  return ((test_communicator *)obj)->free(err);
}

communicator test_communicator::communicator_interface = {
    .send_message = &test_communicator::send_message,
    .free = &test_communicator::free};

void test_communicator::set_ref(communicator_ref *ref) {
  ref->interface = &test_communicator::communicator_interface;
  ref->obj = this;
}

communicator_factory_ref test_communicator_factory::get_factory() {
  return {.interface = &test_communicator_factory::factory_interface,
          .obj = this};
}

communicator_factory test_communicator_factory::factory_interface = {
    .create_communicator = &test_communicator_factory::create_communicator};

bool test_communicator_factory::create_communicator(
    void *obj, cremona_device_manager_t *device_manager, communicator_ref *ref,
    crmna_err_t *err) {
  return ((test_communicator_factory *)obj)->create_communicator(device_manager, ref, err);
}

