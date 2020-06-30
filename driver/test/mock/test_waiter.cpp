#include "test_waiter.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

bool test_waiter::wait(void *obj, cond_func cond, void *context, int msec,
                       crmna_err_t *err) {
  return ((test_waiter *)obj)->wait(cond, context, msec, err);
}
bool test_waiter::notify(void *obj, crmna_err_t *err) {
  return ((test_waiter *)obj)->notify(err);
}
bool test_waiter::free(void *obj, crmna_err_t *err) {
  return ((test_waiter *)obj)->free(err);
}

waiter test_waiter::interface = {.wait = &test_waiter::wait,
                                 .notify = &test_waiter::notify,
                                 .free = &test_waiter::free};

waiter_ref test_waiter::get_ref() {
  return {.interface = &test_waiter::interface, .obj = this};
}

void test_waiter::set_ref(waiter_ref *ref) {
  ref->interface = &test_waiter::interface;
  ref->obj = this;
}

test_waiter_mock::test_waiter_mock() {
  ON_CALL(*this, wait(_, _, _, _))
      .WillByDefault(Invoke(
          [this](cond_func cond, void *context, int msec, crmna_err_t *) {
            {
              std::unique_lock<std::mutex> lk(this->mtx);
              this->cond.wait_for(
                  lk, std::chrono::milliseconds(msec),
                  [this, &cond, &context] { return cond(context); });
            }
            return true;
          }));

  ON_CALL(*this, notify(_)).WillByDefault(Invoke([this](crmna_err_t *) {
    { std::lock_guard<std::mutex> lk(this->mtx); }
    this->cond.notify_one();
    return true;
  }));
}

bool test_waiter_factory::create_waiter(void *obj, waiter_ref *lock,
                                        crmna_err_t *err) {
  return ((test_waiter_factory *)obj)->create_waiter(lock, err);
}

waiter_factory test_waiter_factory::interface = {
    .create_waiter = &test_waiter_factory::create_waiter};

waiter_factory_ref test_waiter_factory::get_factory() {
  return {.interface = &test_waiter_factory::interface, .obj = this};
}

test_waiter_factory_mock::test_waiter_factory_mock() {
  ON_CALL(*this, create_waiter(_, _))
      .WillByDefault(Invoke([this](waiter_ref *ref, crmna_err_t *) {
        if (this->next_mock == 10) {
          throw std::runtime_error("overflow mock");
        }
        this->mocks[this->next_mock].set_ref(ref);
        this->next_mock++;
        return true;
      }));
}