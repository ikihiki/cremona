#pragma once
#include "cremona.h"
#include <gmock/gmock.h>

class test_communicator {
public:
  virtual ~test_communicator() {}
  void set_ref(communicator_ref *ref);
  communicator_ref get_ref();

  virtual int send_message(uint32_t pid, int type, crmna_buf_t *buf,
                           crmna_err *err) = 0;
  virtual void free() = 0;

  communicate_t *communicate;

private:
  static int
  send_message(void *obj, uint32_t pid, int type,
               crmna_buf_t *buf, crmna_err *err);
  static void free(void *obj);

  static communicator communicator_interface;
};

class test_communicator_mock : public test_communicator {
public:
  test_communicator_mock();
  MOCK_METHOD4(send_message,
               int(uint32_t pid, int type, crmna_buf_t *buf, crmna_err *err));
  MOCK_METHOD0(free, void());

  communicator_ref ref;
  communicator_ref *get_mock_ref();
};

class test_communicator_factory {
public:
  communicator_factory_ref get_factory();
  virtual ~test_communicator_factory() {}
  virtual bool create_communicator(communicate_t *communicate,
                                   communicator_ref *ref, crmna_err *err) = 0;

private:
  static bool create_communicator(void *obj, communicate_t *communicate,
                                  communicator_ref *ref, crmna_err *err);
  static communicator_factory factory_interface;
};

class test_communicator_factory_mock : public test_communicator_factory {
public:
  test_communicator_factory_mock();
  MOCK_METHOD3(create_communicator,
               bool(communicate_t *communicate, communicator_ref *ref,
                    crmna_err *err));

  test_communicator_mock mocks[10];
  int next_mock = 0;

  communicator_factory_ref ref;
   communicator_factory_ref *get_mock_factory();
};
