#pragma once
#include "cremona.h"
#include <gmock/gmock.h>

class test_communicator {
public:
  virtual ~test_communicator() {}
  void set_ref(communicator_ref *ref);

  virtual int send_message(uint32_t pid, int type, crmna_buf_t *buf,
                           crmna_err_t *err) = 0;
  virtual bool free(crmna_err_t *err) = 0;

private:
  static int send_message(void *obj, uint32_t pid, int type, crmna_buf_t *buf,
                          crmna_err_t *err);
  static bool free(void *obj, crmna_err_t *err);

  static communicator communicator_interface;
};

class test_communicator_mock : public test_communicator {
public:
  MOCK_METHOD4(send_message,
               int(uint32_t pid, int type,
                   crmna_buf_t *buf, crmna_err_t *err));
  MOCK_METHOD1(free, bool(crmna_err_t *err));
};

class test_communicator_factory {
public:
  communicator_factory_ref get_factory();
  virtual ~test_communicator_factory() {}
  virtual bool create_communicator(cremona_device_manager_t *device_manager,
                                   communicator_ref *ref, crmna_err_t *err) = 0;

private:
  static bool create_communicator(void *obj,
                                  cremona_device_manager_t *device_manager,
                                  communicator_ref *ref, crmna_err_t *err);
  static communicator_factory factory_interface;
};

class test_communicator_factory_mock : public test_communicator_factory {
public:
  MOCK_METHOD3(create_communicator,
               bool(cremona_device_manager_t *device_manager,
                    communicator_ref *ref, crmna_err_t *err));
};

