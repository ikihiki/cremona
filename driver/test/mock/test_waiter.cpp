#pragma once
#include "test_waiter.h"

bool test_waiter::wait(void *obj, crmna_err_t *err) {
  return ((test_waiter *)obj)->wait(err);
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

waiter_ref test_waiter::get_ref(){
    return {.interface = &test_waiter::interface,
            .obj = this};
}

bool test_waiter_factory::create_waiter(void *obj, waiter_ref *lock,
                                        crmna_err_t *err) {
    return ((test_waiter_factory *)obj)->create_waiter(lock, err);
}

waiter_factory test_waiter_factory::interface = {
    .create_waiter = &test_waiter_factory::create_waiter};

waiter_factory_ref test_waiter_factory::get_factory(){
    return {.interface = &test_waiter_factory::interface,
            .obj = this};
}
