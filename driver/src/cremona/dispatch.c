#include "central_store.h"
#include "error.h"
#include "message.h"

bool dispatch(store_t *store, action_t *action, crmna_err_t *err) {
  switch (action->type) {
  case CREATE_DEVICE:
    return create_device(store, action, err);

  default:
    break;
  }
}

bool create_action_from_message(int type, int pid, crmna_buf_t *message,
                                action_t *action, crmna_err_t *err) {
  switch (type) {
  case CRMNA_CREATE_DEVICE:
    return create_action_from_create_device_message(pid, message, action, err);

  default:
    break;
  }
}