#include "central_store.h"
#include "error.h"
#include "message.h"

bool dispatch(store_t *store, action_t *action, crmna_err_t *err) {
  switch (action->type) {
  case CREATE_DEVICE:
    return create_device(store, action, err);
  case CREATE_TOOT:
    return create_toot(store, action, err);
  case CREATE_TOOT_RESULT:
    return create_toot_result(store, action, err);
  case ADD_TOOT_ELEMENT:
    return add_toot_element(store, action, err);
  case ADD_TOOT_ELEMENT_RESULT:
    return add_toot_element_result(store, action, err);
  case SEND_TOOT:
    return send_toot(store, action, err);
  case SEND_TOOT_RESULT:
    return send_toot_result(store, action, err);
  case DESTROY_DEVICE:
    return destroy_device(store, action, err);
  default:
    return false;
  }
}

bool create_action_from_message(int type, int pid, crmna_buf_t *message,
                                action_t *action, crmna_err_t *err) {
  switch (type) {
  case CRMNA_CREATE_DEVICE:
    return create_action_from_create_device_message(pid, message, action, err);
  case CRMNA_DESTROY_DEVICE:
    return create_action_from_destroy_device_message(pid, message, action, err);
  case CRMNA_NEW_TOOT_RESULT:
    return create_action_from_create_toot_result_message(pid, message, action,
                                                         err);
  case CRMNA_ADD_TOOT_TEXT_RESULT:
    return create_action_from_add_toot_element_result_message(pid, message,
                                                              action, err);
  case CRMNA_SEND_TOOT_RESULT:
    return create_action_from_send_toot_result_message(pid, message, action,
                                                       err);
  default:
    return false;
  }
}