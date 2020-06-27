#pragma once
#include <gmock/gmock.h>
#include "interfaces/locker_factory.h"

class test_locker {
public:
  virtual ~test_locker(){};
  void set_ref(locker_ref *ref);
  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual void free() = 0;

private:
  static void lock(void *obj);
  static void unlock(void *obj);
  static void free(void *obj);

  static locker interface;
};

class test_locker_mock : public test_locker {
public:
  test_locker_mock();
  MOCK_METHOD0(lock, void());
  MOCK_METHOD0(unlock, void());
  MOCK_METHOD0(free, void());
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