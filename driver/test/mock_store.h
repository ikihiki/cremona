#pragma once
#include "central_store.h"
#include <gmock/gmock.h>
class device {};

class mock_store_base {
public:
  std::map<int, device> devices;
  virtual ~mock_store_base() {}
  virtual bool add_device(int pid, int uid, char *name, int *id,
                          crmna_err_t *err) = 0;
  virtual void remove_device(int device_id) = 0;
  virtual bool attach_device_class(int device_id, crmna_err_t *err) = 0;
  virtual void detach_device_class(int device_id) = 0;
  virtual void set_device_ready(int device_id) = 0;

  virtual communicator_ref_t get_communicator() = 0;
};

class mock_store : public mock_store_base {
public:
  MOCK_METHOD5(add_device,
               bool(int pid, int uid, char *name, int *id, crmna_err_t *err));
  MOCK_METHOD1(remove_device, void(int device_id));
  MOCK_METHOD2(attach_device_class, bool(int device_id, crmna_err_t *err));
  MOCK_METHOD1(detach_device_class, void(int device_id));
  MOCK_METHOD1(set_device_ready, void(int device_id));
  MOCK_METHOD0(get_communicator, communicator_ref_t());
};

typedef struct store {
  mock_store_base *mock;
} store_t;

#define DEFINE_TEST_STORE                                                      \
  store_t store;                                                               \
  mock_store mock_store;                                                       \
  store.mock = &mock_store;