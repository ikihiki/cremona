#pragma once
#include "interfaces/waiter_factory.h"
#include <gmock/gmock.h>

class test_waiter {
public:
  virtual ~test_waiter() {}
  waiter_ref get_ref();

  virtual bool wait(crmna_err_t *err) = 0;
  virtual bool notify(crmna_err_t *err) = 0;
  virtual bool free(crmna_err_t *err) = 0;

private:
  static bool wait(void *obj, crmna_err_t *err);
  static bool notify(void *obj, crmna_err_t *err);
  static bool free(void *obj, crmna_err_t *err);

  static waiter interface;
};

class test_waiter_mock : public test_waiter {
public:
  MOCK_METHOD1(wait, bool(crmna_err_t *err));
  MOCK_METHOD1(notify, bool(crmna_err_t *err));
  MOCK_METHOD1(free, bool(crmna_err_t *err));
};

class test_waiter_factory {
public:
  virtual bool create_waiter(waiter_ref *lock, crmna_err_t *err) = 0;
  ~test_waiter_factory() {}
  waiter_factory_ref get_factory();

private:
  static bool create_waiter(void *obj, waiter_ref *lock, crmna_err_t *err);

  static waiter_factory interface;
};

class test_waiter_factory_mock : public test_waiter_factory {
  public:
  MOCK_METHOD2(create_waiter, bool(waiter_ref *lock, crmna_err_t *err));
};