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

  virtual bool add_toot(int device_id, int *toot_id, crmna_err_t *err) = 0;
  virtual void remove_toot(int toot_id) = 0;
  virtual bool wait_toot_ready_or_failer(int toot_id, crmna_err_t *err) = 0;
  virtual bool wait_toot_sent_or_failer(int toot_id, crmna_err_t *err) = 0;
  virtual void set_toot_failer(int toot_id) = 0;
  virtual void set_toot_ready(int toot_id) = 0;
  virtual void set_toot_sent(int toot_id) = 0;
  virtual int get_device_id_from_toot(int toot_id) = 0;
  virtual int get_device_pid_from_toot(int toot_id) = 0;
  virtual bool add_element(int toot_id, int *element_id, crmna_err_t *err) = 0;
  virtual void remove_element(int element_id) = 0;
  virtual bool wait_element_sent_or_failer(int element_id,
                                           crmna_err_t *err) = 0;
  virtual void set_element_sent(int element_id) = 0;
  virtual void set_element_failer(int element_id) = 0;
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
  MOCK_METHOD3(add_toot, bool(int device_id, int *toot_id, crmna_err_t *err));
  MOCK_METHOD1(remove_toot, void(int toot_id));
  MOCK_METHOD2(wait_toot_ready_or_failer, bool(int toot_id, crmna_err_t *err));
  MOCK_METHOD2(wait_toot_sent_or_failer, bool(int toot_id, crmna_err_t *err));
  MOCK_METHOD1(set_toot_failer, void(int toot_id));
  MOCK_METHOD1(set_toot_ready, void(int toot_id));
  MOCK_METHOD1(set_toot_sent, void(int toot_id));
  MOCK_METHOD1(get_device_id_from_toot, int(int toot_id));
  MOCK_METHOD1(get_device_pid_from_toot, int(int toot_id));
  MOCK_METHOD3(add_element,
               bool(int toot_id, int *element_id, crmna_err_t *err));
  MOCK_METHOD1(remove_element, void(int element_id));
  MOCK_METHOD2(wait_element_sent_or_failer,
               bool(int element_id, crmna_err_t *err));
  MOCK_METHOD1(set_element_sent, void(int element_id));
  MOCK_METHOD1(set_element_failer, void(int element_id));
};

typedef struct store {
  mock_store_base *mock;
} store_t;

#define DEFINE_TEST_STORE                                                      \
  store_t store;                                                               \
  mock_store mock_store;                                                       \
  store.mock = &mock_store;
  