#include "test_device_file.h"
using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

void test_device_file::free(void *obj) { ((test_device_file *)obj)->free(); }

device_file test_device_file::interface = {.free = &test_device_file::free};

void test_device_file::set_ref(device_file_ref *ref) {
  ref->interface = &test_device_file::interface;
  ref->obj = this;
}

bool test_device_file_factory::create_device_file(void *obj,
                                                  cremona_device_t *device,
                                                  device_file_ref *device_file,
                                                  crmna_err *err) {
  return ((test_device_file_factory *)obj)
      ->create_device_file(device, device_file, err);
}

device_file_factory test_device_file_factory::interface = {
    .create_device_file = &test_device_file_factory::create_device_file};

device_file_factory_ref test_device_file_factory::get_factory() {
  return {.interface = &test_device_file_factory::interface, .obj = this};
}

test_device_file_factory_mock::test_device_file_factory_mock() {
  this->ref = this->get_factory();

  ON_CALL(*this, create_device_file(_, _, _))
      .WillByDefault(
          Invoke([this](cremona_device_t *, device_file_ref *ref, crmna_err *) {
            if (this->next_mock == 10) {
              throw std::runtime_error("overflow mock");
            }
            this->mocks[this->next_mock].set_ref(ref);
            this->next_mock++;
            return true;
          }));
}

device_file_factory_ref *test_device_file_factory_mock::get_mock_factory(){
  return &this->ref;
}