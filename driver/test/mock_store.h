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
  virtual void remove_device(uint32_t device_id) = 0;
  virtual bool attach_device_class(uint32_t device_id, crmna_err_t *err) = 0;
  virtual void detach_device_class(uint32_t device_id) = 0;
  virtual void set_device_ready(uint32_t device_id) = 0;

  virtual communicator_ref_t get_communicator() = 0;

  virtual bool add_toot(uint32_t device_id, uint32_t *toot_id,
                        crmna_err_t *err) = 0;
  virtual void remove_toot(uint32_t toot_id) = 0;
  virtual bool wait_toot_ready_or_failer(uint32_t toot_id,
                                         crmna_err_t *err) = 0;
  virtual bool wait_toot_sent_or_failer(uint32_t toot_id, crmna_err_t *err) = 0;
  virtual void set_toot_failer(uint32_t toot_id) = 0;
  virtual void set_toot_ready(uint32_t toot_id) = 0;
  virtual void set_toot_sent(uint32_t toot_id) = 0;
  virtual int get_device_id_from_toot(uint32_t toot_id) = 0;
  virtual int get_device_pid_from_toot(uint32_t toot_id) = 0;
  virtual bool add_element(uint32_t toot_id, uint32_t *element_id,
                           crmna_err_t *err) = 0;
  virtual void remove_element(uint32_t element_id) = 0;
  virtual bool wait_element_sent_or_failer(uint32_t element_id,
                                           crmna_err_t *err) = 0;
  virtual void set_element_sent(uint32_t element_id) = 0;
  virtual void set_element_failer(uint32_t element_id) = 0;
};

class mock_store : public mock_store_base {
public:
  MOCK_METHOD5(add_device,
               bool(int pid, int uid, char *name, int *id, crmna_err_t *err));
  MOCK_METHOD1(remove_device, void(uint32_t device_id));
  MOCK_METHOD2(attach_device_class, bool(uint32_t device_id, crmna_err_t *err));
  MOCK_METHOD1(detach_device_class, void(uint32_t device_id));
  MOCK_METHOD1(set_device_ready, void(uint32_t device_id));
  MOCK_METHOD0(get_communicator, communicator_ref_t());
  MOCK_METHOD3(add_toot,
               bool(uint32_t device_id, uint32_t *toot_id, crmna_err_t *err));
  MOCK_METHOD1(remove_toot, void(uint32_t toot_id));
  MOCK_METHOD2(wait_toot_ready_or_failer,
               bool(uint32_t toot_id, crmna_err_t *err));
  MOCK_METHOD2(wait_toot_sent_or_failer,
               bool(uint32_t toot_id, crmna_err_t *err));
  MOCK_METHOD1(set_toot_failer, void(uint32_t toot_id));
  MOCK_METHOD1(set_toot_ready, void(uint32_t toot_id));
  MOCK_METHOD1(set_toot_sent, void(uint32_t toot_id));
  MOCK_METHOD1(get_device_id_from_toot, int(uint32_t toot_id));
  MOCK_METHOD1(get_device_pid_from_toot, int(uint32_t toot_id));
  MOCK_METHOD3(add_element,
               bool(uint32_t toot_id, uint32_t *element_id, crmna_err_t *err));
  MOCK_METHOD1(remove_element, void(uint32_t element_id));
  MOCK_METHOD2(wait_element_sent_or_failer,
               bool(uint32_t element_id, crmna_err_t *err));
  MOCK_METHOD1(set_element_sent, void(uint32_t element_id));
  MOCK_METHOD1(set_element_failer, void(uint32_t element_id));
};

typedef struct store {
  mock_store_base *mock;
} store_t;

#define DEFINE_TEST_STORE                                                      \
  store_t store;                                                               \
  mock_store mock_store;                                                       \
  store.mock = &mock_store;
