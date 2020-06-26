#include "commuinicator.h"

int communicator_send_message(communicator_ref *ref, uint32_t pid, int type,
                              crmna_buf_t *data, crmna_err_t *err) {
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return -1;
  }
  if (ref->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return -1;
  }
  if (ref->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return -1;
  }
  if (ref->interface->send_message == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
             "function pointer \"send_message\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return -1;
  }
  return ref->interface->send_message(ref->obj, pid, type, data, err);
}

bool communicator_free(communicator_ref *ref, crmna_err_t *err) {
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->free == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
             "function pointer \"free\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->free(ref->obj, err);
}

void clear_communicator_ref(communicator_ref *ref) {
  ref->interface = NULL;
  ref->obj = NULL;
}