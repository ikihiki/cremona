#include "commuinicator.h"

int communicator_send_message(communicator_ref_t *ref, uint32_t pid,
                                     int type, crmna_buf_t *data,
                                     crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return -1;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return -1;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return -1;
  }
  if (ref->interface->send_message == NULL) {
    err->error_msg = "function pointer \"send_message\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return -1;
  }
  return ref->interface->send_message(ref->obj, pid, type, data, err);
}

bool communicator_register_reciver_func(communicator_ref_t *ref,
                                               reciver_func *func,
                                               crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->register_reciver_func == NULL) {
    err->error_msg = "function pointer \"register_reciver_func\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->register_reciver_func(ref->obj, func, err);
}

bool communicator_free(communicator_ref_t *ref, crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->free == NULL) {
    err->error_msg = "function pointer \"free\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->free(ref->obj, err);
}

bool create_communicator(communicator_factory_t *factory,
                                communicator_ref_t *ref, crmna_err_t *err) {
  if (factory == NULL) {
    err->error_msg = "factory is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref == NULL) {
    err->error_msg = "output target reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  ref->interface = NULL;
  ref->obj = NULL;

  return factory->create_communicator(ref, err);
}
