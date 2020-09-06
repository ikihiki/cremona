#pragma once
#include "commuinicator.h"
#include <gmock/gmock.h>

class test_communicator {
public:
  virtual ~test_communicator() {}
  void set_ref(communicator_ref_t *ref);
  communicator_ref_t get_ref();

  virtual size_t send_message(uint32_t pid, int type, crmna_buf_t *buf,
                              crmna_err_t *err) = 0;
  virtual void free() = 0;

private:
  static size_t send_message(void *obj, uint32_t pid, int type,
                             crmna_buf_t *buf, crmna_err_t *err);
  static void free(void *obj);

  static communicator_t communicator_interface;
};

class test_communicator_mock : public test_communicator {
public:
  test_communicator_mock();
  MOCK_METHOD4(send_message, size_t(uint32_t pid, int type, crmna_buf_t *buf,
                                    crmna_err_t *err));
  MOCK_METHOD0(free, void());

  communicator_ref_t ref;
  communicator_ref_t get_mock_ref();
};
