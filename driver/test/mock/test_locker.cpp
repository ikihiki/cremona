#include "test_locker.h"
using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

void test_locker::lock(void *obj) { ((test_locker *)obj)->lock(); }
void test_locker::unlock(void *obj) { ((test_locker *)obj)->unlock(); }
void test_locker::free(void *obj) { ((test_locker *)obj)->free(); }

locker test_locker::interface = {.lock = &test_locker::lock,
                                 .unlock = &test_locker::unlock,
                                 .free = &test_locker::free};

void test_locker::set_ref(locker_ref *ref) {
  ref->interface = &test_locker::interface;
  ref->obj = this;
}

test_locker_mock::test_locker_mock() {
  ON_CALL(*this, free()).WillByDefault(Return());
}

bool test_locker_factory::create_locker(void *obj, locker_ref *lock,
                                        crmna_err *err) {
  return ((test_locker_factory *)obj)->create_locker(lock, err);
}

locker_factory test_locker_factory::interface = {
    .create_locker = &test_locker_factory::create_locker};

locker_factory_ref test_locker_factory::get_factory() {
  return {.interface = &test_locker_factory::interface, .obj = this};
}

test_locker_factory_mock::test_locker_factory_mock() {
  this->ref = this->get_factory();

  ON_CALL(*this, create_locker(_, _))
      .WillByDefault(Invoke([this](locker_ref *ref, crmna_err *) {
        if (this->next_mock == 10) {
          throw std::runtime_error("overflow mock");
        }
        this->mocks[this->next_mock].set_ref(ref);
        this->next_mock++;
        return true;
      }));
}

locker_factory_ref *test_locker_factory_mock::get_mock_factory(){
  return &this->ref;
}