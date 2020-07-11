#include "test_communicator.h"
#include <iostream>
#include <string>
using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

int test_communicator::send_message(void *obj, uint32_t pid, int type,
                                    crmna_buf_t *buf, crmna_err *err) {
  return ((test_communicator *)obj)->send_message(pid, type, buf, err);
}
void test_communicator::free(void *obj) { ((test_communicator *)obj)->free(); }

communicator test_communicator::communicator_interface = {
    .send_message = &test_communicator::send_message,
    .free = &test_communicator::free};

void test_communicator::set_ref(communicator_ref *ref) {
  ref->interface = &test_communicator::communicator_interface;
  ref->obj = this;
}

communicator_ref test_communicator::get_ref() {
  return {.interface = &test_communicator::communicator_interface, .obj = this};
}

test_communicator_mock::test_communicator_mock() {
  this->ref = this->get_ref();
  ON_CALL(*this, free()).WillByDefault(Return());
  ON_CALL(*this, send_message(_, _, _, _))
      .WillByDefault(Invoke([](int32_t pid, int type, crmna_buf_t *buf,
                               crmna_err *err) { return (int)buf->buf_size; }));
}

communicator_ref *test_communicator_mock::get_mock_ref() { return &this->ref; }

communicator_factory_ref test_communicator_factory::get_factory() {
  return {.interface = &test_communicator_factory::factory_interface,
          .obj = this};
}

communicator_factory test_communicator_factory::factory_interface = {
    .create_communicator = &test_communicator_factory::create_communicator};

bool test_communicator_factory::create_communicator(void *obj,
                                                    communicate_t *communicate,
                                                    communicator_ref *ref,
                                                    crmna_err *err) {
  return ((test_communicator_factory *)obj)
      ->create_communicator(communicate, ref, err);
}

test_communicator_factory_mock::test_communicator_factory_mock() {
  this->ref = this->get_factory();

  ON_CALL(*this, create_communicator(_, _, _))
      .WillByDefault(Invoke([this](communicate_t *communicate,
                                   communicator_ref *ref, crmna_err *) {
        if (this->next_mock == 10) {
          throw std::runtime_error("overflow mock");
        }
        this->mocks[this->next_mock].communicate = communicate;
        this->mocks[this->next_mock].set_ref(ref);
        this->next_mock++;
        return true;
      }));
}

communicator_factory_ref *test_communicator_factory_mock::get_mock_factory() {
  return &this->ref;
}
