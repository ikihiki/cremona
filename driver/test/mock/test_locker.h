#pragma once
#include <gmock/gmock.h>
#include "interfaces/locker_factory.h"

class test_locker {
public:
  virtual ~test_locker(){};
  void set_ref(locker_ref *ref);
  virtual bool lock(crmna_err_t *err) = 0;
  virtual bool unlock(crmna_err_t *err) = 0;
  virtual bool free(crmna_err_t *err) = 0;

private:
  static bool lock(void *obj, crmna_err_t *err);
  static bool unlock(void *obj, crmna_err_t *err);
  static bool free(void *obj, crmna_err_t *err);

  static locker interface;
};

class test_locker_mock : public test_locker {
public:
  test_locker_mock();
  MOCK_METHOD1(lock, bool(crmna_err_t *err));
  MOCK_METHOD1(unlock, bool(crmna_err_t *err));
  MOCK_METHOD1(free, bool(crmna_err_t *err));
};

class test_locker_factory {
public:
  virtual ~test_locker_factory() {}
  locker_factory_ref get_factory();
  virtual bool create_locker(locker_ref *lock, crmna_err_t *err) = 0;

private:
  static bool create_locker(void *obj, locker_ref *lock, crmna_err_t *err);

  static locker_factory interface;
};

class test_locker_factory_mock: public test_locker_factory{
  public:
    test_locker_factory_mock();
    MOCK_METHOD2(create_locker, bool(locker_ref *lock, crmna_err_t *err));

    test_locker_mock mocks[10];
    int next_mock = 0;
};