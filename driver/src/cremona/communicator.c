#include "commuinicator.h"

bool communicator_send_message(communicator_ref_t *ref, uint32_t pid,
                                 int type, crmna_buf_t *data,
                                 crmna_err_t *err) {
  if (ref == NULL) {
    ADD_ERROR(err, "reference is null");
    return -1;
  }
  if (ref->interface == NULL) {
    ADD_ERROR(err, "interface is null");
    return -1;
  }
  if (ref->obj == NULL) {
    ADD_ERROR(err, "interface reference is null");
    return -1;
  }
  if (ref->interface->send_message == NULL) {
    ADD_ERROR(err, "function pointer \"send_message\" is null");
    return -1;
  }
  return ref->interface->send_message(ref->obj, pid, type, data, err);
}

void communicator_free(communicator_ref_t *ref) {
  if (ref == NULL) {
    return;
  }
  if (ref->interface != NULL) {
    ref->interface->free(ref->obj);
  }
  ref->interface = NULL;
  ref->obj = NULL;
}

void clear_communicator_ref(communicator_ref_t *ref) {
  ref->interface = NULL;
  ref->obj = NULL;
}