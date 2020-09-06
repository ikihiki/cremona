#include "mock_communicator.h"
#include <iostream>
#include <string>
using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

size_t test_communicator::send_message(void *obj, uint32_t pid, int type,
                                       crmna_buf_t *buf, crmna_err_t *err) {
  return ((test_communicator *)obj)->send_message(pid, type, buf, err);
}
void test_communicator::free(void *obj) { ((test_communicator *)obj)->free(); }

communicator_t test_communicator::communicator_interface = {
    .send_message = &test_communicator::send_message,
    .free = &test_communicator::free};

void test_communicator::set_ref(communicator_ref_t *ref) {
  ref->interface = &test_communicator::communicator_interface;
  ref->obj = this;
}

communicator_ref_t test_communicator::get_ref() {
  return {.interface = &test_communicator::communicator_interface, .obj = this};
}

test_communicator_mock::test_communicator_mock() {
  this->ref = this->get_ref();
  ON_CALL(*this, free()).WillByDefault(Return());
  ON_CALL(*this, send_message(_, _, _, _))
      .WillByDefault(Invoke([](int32_t pid, int type, crmna_buf_t *buf,
                               crmna_err_t *err) { return (int)buf->buf_size; }));
}

communicator_ref_t test_communicator_mock::get_mock_ref() { return this->ref; }

