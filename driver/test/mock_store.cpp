#include "mock_store.h"
#include "central_store.h"

bool add_device(store_t *store, int pid, int uid, char *name, int *id,
                crmna_err_t *err) {
  return store->mock->add_device(pid, uid, name, id, err);
}

bool attach_device_class(store_t *store, uint32_t device_id, crmna_err_t *err) {
  return store->mock->attach_device_class(device_id, err);
}
void remove_device(store_t *store, uint32_t device_id) {
  store->mock->remove_device(device_id);
}

void detach_device_class(store_t *store, uint32_t device_id) {
  store->mock->detach_device_class(device_id);
}
void set_device_ready(store_t *store, uint32_t device_id) {
  store->mock->set_device_ready(device_id);
}
communicator_ref_t get_communicator(store_t *store) {
  return store->mock->get_communicator();
}

bool add_toot(store_t *store, uint32_t device_id, uint32_t *toot_id,
              crmna_err_t *err) {
  return store->mock->add_toot(device_id, toot_id, err);
}
void remove_toot(store_t *store, uint32_t toot_id) {
  store->mock->remove_toot(toot_id);
}
bool wait_toot_ready_or_failer(store_t *store, uint32_t toot_id,
                               crmna_err_t *err) {
  return store->mock->wait_toot_ready_or_failer(toot_id, err);
}
bool wait_toot_sent_or_failer(store_t *store, uint32_t toot_id,
                              crmna_err_t *err) {
  return store->mock->wait_toot_sent_or_failer(toot_id, err);
}
void set_toot_failer(store_t *store, uint32_t toot_id) {
  store->mock->set_toot_failer(toot_id);
}
void set_toot_ready(store_t *store, uint32_t toot_id) {
  store->mock->set_toot_ready(toot_id);
}
void set_toot_sent(store_t *store, uint32_t toot_id) {
  store->mock->set_toot_sent(toot_id);
}
int get_device_id_from_toot(store_t *store, uint32_t toot_id) {
  return store->mock->get_device_id_from_toot(toot_id);
}
int get_device_pid_from_toot(store_t *store, uint32_t toot_id) {
  return store->mock->get_device_pid_from_toot(toot_id);
}

bool add_element(store_t *store, uint32_t toot_id, uint32_t *element_id,
                 crmna_err_t *err) {
  return store->mock->add_element(toot_id, element_id, err);
}
void remove_element(store_t *store, uint32_t element_id) {
  store->mock->remove_element(element_id);
}
bool wait_element_sent_or_failer(store_t *store, uint32_t element_id,
                                 crmna_err_t *err) {
  return store->mock->wait_element_sent_or_failer(element_id, err);
}
void set_element_sent(store_t *store, uint32_t element_id) {
  store->mock->set_element_sent(element_id);
}
void set_element_failer(store_t *store, uint32_t element_id) {
  store->mock->set_element_failer(element_id);
}