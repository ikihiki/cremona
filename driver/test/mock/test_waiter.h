#pragma once
#include "interfaces/waiter_factory.h"
#include <condition_variable>
#include <gmock/gmock.h>

class test_waiter {
public:
  virtual ~test_waiter() {}
  waiter_ref get_ref();
  void set_ref(waiter_ref *ref);

  virtual bool wait(cond_func cond, void *context, int msec,
                    crmna_err *err) = 0;
  virtual void notify() = 0;
  virtual void free() = 0;

private:
  static bool wait(void *obj, cond_func cond, void *context, int msec,
                   crmna_err *err);
  static void notify(void *obj);
  static void free(void *obj);

  static waiter interface;
};

class test_waiter_mock : public test_waiter {
public:
  test_waiter_mock();
  MOCK_METHOD4(wait,
               bool(cond_func cond, void *context, int msec, crmna_err *err));
  MOCK_METHOD0(notify, void());
  MOCK_METHOD0(free, void());

  void *context;
  std::mutex mtx;
  std::condition_variable cond;
};

class test_waiter_factory {
public:
  virtual bool create_waiter(waiter_ref *lock, crmna_err *err) = 0;
  ~test_waiter_factory() {}
  waiter_factory_ref get_factory();

private:
  static bool create_waiter(void *obj, waiter_ref *lock, crmna_err *err);

  static waiter_factory interface;
};

class test_waiter_factory_mock : public test_waiter_factory {
public:
  test_waiter_factory_mock();
  MOCK_METHOD2(create_waiter, bool(waiter_ref *lock, crmna_err *err));

  test_waiter_mock mocks[10];
  int next_mock = 0;

  waiter_factory_ref ref;
  waiter_factory_ref *get_mock_factory();
};