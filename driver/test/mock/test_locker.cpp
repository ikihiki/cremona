#include "test_locker.h"
using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

bool test_locker::lock(void *obj, crmna_err_t *err) {
  return ((test_locker *)obj)->lock(err);
}
bool test_locker::unlock(void *obj, crmna_err_t *err) {
  return ((test_locker *)obj)->unlock(err);
}
bool test_locker::free(void *obj, crmna_err_t *err) {
  return ((test_locker *)obj)->free(err);
}

locker test_locker::interface = {.lock = &test_locker::lock,
                                 .unlock = &test_locker::unlock,
                                 .free = &test_locker::free};

void test_locker::set_ref(locker_ref *ref) {
  ref->interface = &test_locker::interface;
  ref->obj = this;
}

test_locker_mock::test_locker_mock(){
  ON_CALL(*this, free(_)).WillByDefault(Return(true));
}

    bool
    test_locker_factory::create_locker(void *obj, locker_ref *lock,
                                       crmna_err_t *err) {
  return ((test_locker_factory *)obj)->create_locker(lock, err);
}

locker_factory test_locker_factory::interface = {
    .create_locker = &test_locker_factory::create_locker};

locker_factory_ref test_locker_factory::get_factory(){
    return {.interface = &test_locker_factory::interface,
            .obj = this};
}

test_locker_factory_mock::test_locker_factory_mock(){
  ON_CALL(*this, create_locker(_, _))
      .WillByDefault(
          DoAll(Invoke([this](locker_ref *ref, crmna_err_t *) {
                  if (this->next_mock == 10) {
                    throw std::runtime_error("overflow mock");
                  }
                  this->mocks[this->next_mock].set_ref(ref);
                  this->next_mock++;
                }),
                Return(true)));
}