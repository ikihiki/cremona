#include <iostream>
#include <string>
#include "test_communicator.h"
using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

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

test_communicator_mock::test_communicator_mock(){
  ON_CALL(*this, free(_)).WillByDefault(Return(true));
}

    communicator_factory_ref
    test_communicator_factory::get_factory() {
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

test_communicator_factory_mock::test_communicator_factory_mock(){
  ON_CALL(*this, create_communicator(_, _, _))
      .WillByDefault(
          DoAll(Invoke([this](cremona_device_manager_t *,
                              communicator_ref *ref, crmna_err_t *) {
                  if (this->next_mock == 10) {
                    throw std::runtime_error("overflow mock");
                  }
                  this->mocks[this->next_mock].set_ref(ref);
                  this->next_mock++;
                }),
                Return(true)));
}