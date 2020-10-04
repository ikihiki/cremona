#include "central_store.h"
#include "message.h"

action_t create_action_create_toot(uint32_t device_id,
                                   set_toot_id_func set_toot_id,
                                   void *set_toot_id_context) {
  action_t action;
  action.type = CREATE_TOOT;
  action.payload.create_toot.device_id = device_id;
  action.payload.create_toot.set_toot_id = set_toot_id;
  action.payload.create_toot.set_toot_id_context = set_toot_id_context;
  return action;
}

bool create_toot(store_t *store, action_t *action, crmna_err_t *err) {
  uint32_t toot_id;
  if (!add_toot(store, action->payload.create_toot.device_id, &toot_id, err)) {
    return false;
  }

  new_toot_t msg;
  msg.device_id = action->payload.create_toot.device_id;
  msg.toot_id = toot_id;
  DEFINE_CRMNA_BUF(buf, 10)
  if (!serialize_new_toot(&msg, &buf)) {
    remove_toot(store, toot_id);
    return false;
  }

  communicator_ref_t com = get_communicator(store);
  if (!communicator_send_message(&com, action->payload.create_device.pid,
                                CRMNA_NEW_TOOT, &buf, err)) {
    remove_toot(store, toot_id);
    return false;
  }
  
  if (!wait_toot_ready_or_failer(store, toot_id, err)) {
    remove_toot(store, toot_id);
    return false;
  }
  if(!action->payload.create_toot.set_toot_id(toot_id, action->payload.create_toot.set_toot_id_context, err)){
    remove_toot(store, toot_id);
    return false;
  }
  return true;
}
bool create_action_from_create_toot_result_message(int pid,
                                                   crmna_buf_t *message,
                                                   action_t *action,
                                                   crmna_err_t *err) {
  new_toot_result_t payload;
  if (!deserialize_new_toot_result(message, &payload)) {
    return false;
  }
  action->type = CREATE_TOOT_RESULT;
  action->payload.create_toot_result.pid = pid;
  action->payload.create_toot_result.device_id = payload.device_id;
  action->payload.create_toot_result.toot_id = payload.toot_id;
  action->payload.create_toot_result.result = payload.result;
  return true;
}
bool create_toot_result(store_t *store, action_t *action, crmna_err_t *err) {
  uint32_t device_id;

  if (get_device_id_from_toot(store, action->payload.create_toot_result.toot_id,
                              &device_id, err)) {
    return false;
  }
  int pid;
  if (get_device_pid_from_toot(
          store, action->payload.create_toot_result.toot_id, &pid, err)) {
    return false;
  }
  if (pid == action->payload.create_toot_result.pid &&
      device_id != action->payload.create_toot_result.device_id) {
    return false;
  }
  if (action->payload.create_toot_result.result != 0) {
    set_toot_failer(store, action->payload.create_toot_result.toot_id);
    return false;
  }
  else
  {
    set_toot_ready(store, action->payload.create_toot_result.toot_id);
    return true;
  }
}

action_t create_action_add_toot_element(uint32_t toot_id, crmna_buf_t *txet) {
  action_t action;
  action.type = ADD_TOOT_ELEMENT;
  action.payload.add_toot_element.toot_id = toot_id;
  action.payload.add_toot_element.text = txet;
  return action;
}
bool add_toot_element(store_t *store, action_t *action, crmna_err_t *err) {
  uint32_t element_id;
  uint32_t index;
  if (!add_element(store, action->payload.add_toot_element.toot_id, &element_id,
                   &index, err)) {
    return false;
  }
  uint32_t device_id;
  if (get_device_id_from_toot(store, action->payload.create_toot_result.toot_id,
                              &device_id, err)) {
    return false;
  }

  add_toot_text_t msg;
  msg.device_id = device_id;
  msg.toot_id = action->payload.add_toot_element.toot_id;
  msg.element_id = element_id;
  msg.index = index;
  msg.text = action->payload.add_toot_element.text->buf;

  DEFINE_CRMNA_BUF(buf, 10)
  if (!serialize_add_toot_element(&msg, &buf)) {
    remove_element(store, element_id);
    return false;
  }

  communicator_ref_t com = get_communicator(store);
  if (!communicator_send_message(&com, action->payload.create_device.pid,
                                CRMNA_ADD_TOOT_TEXT, &buf, err)) {
    remove_element(store, element_id);
    return false;
  }

  if (!wait_element_sent_or_failer(store, element_id, err)) {
    remove_element(store, element_id);
    return false;
  }
  return true;
}

bool create_action_from_add_toot_element_result_message(int pid,
                                                        crmna_buf_t *message,
                                                        action_t *action,
                                                        crmna_err_t *err) {
  add_toot_text_result_t payload;
  if (!deserialize_add_toot_element_result(message, &payload)) {
    return false;
  }
  action->type = ADD_TOOT_ELEMENT_RESULT;
  action->payload.add_toot_element_result.pid = pid;
  action->payload.add_toot_element_result.device_id = payload.device_id;
  action->payload.add_toot_element_result.toot_id = payload.toot_id;
  action->payload.add_toot_element_result.result = payload.result;
  return true;
}
bool add_toot_element_result(store_t *store, action_t *action,
                             crmna_err_t *err) {
  uint32_t device_id;
  if (get_device_id_from_toot(store, action->payload.create_toot_result.toot_id,
                              &device_id, err)) {
    return false;
  }
  int pid;
  if (get_device_pid_from_toot(
          store, action->payload.create_toot_result.toot_id, &pid, err)) {
    return false;
  }
  if (pid == action->payload.add_toot_element_result.pid &&
      device_id != action->payload.add_toot_element_result.device_id) {
    return false;
  }
  if (action->payload.add_toot_element_result.result != 0) {
    set_element_sent(store, action->payload.add_toot_element_result.element_id);
  } else {
    set_element_failer(store,
                       action->payload.add_toot_element_result.element_id);
  }

  return true;
}

action_t create_action_send_toot(uint32_t toot_id) {
  action_t action;
  action.type = SEND_TOOT;
  action.payload.send_toot.toot_id = toot_id;
  return action;
}
bool send_toot(store_t *store, action_t *action, crmna_err_t *err) {
  uint32_t device_id;
  if (get_device_id_from_toot(store, action->payload.create_toot_result.toot_id,
                              &device_id, err)) {
    return false;
  }
  send_toot_t msg;
  msg.device_id = device_id;
  msg.toot_id = action->payload.add_toot_element.toot_id;

  DEFINE_CRMNA_BUF(buf, 10)
  if (!serialize_send_toot(&msg, &buf)) {
    remove_toot(store, action->payload.send_toot.toot_id);
    return false;
  }

  communicator_ref_t com = get_communicator(store);
  if (!communicator_send_message(&com, action->payload.create_device.pid,
                                CRMNA_SEND_TOOT, &buf, err)) {
    remove_toot(store, action->payload.send_toot.toot_id);
    return false;
  }

  if (!wait_toot_sent_or_failer(store, action->payload.send_toot.toot_id,
                                err)) {
    remove_toot(store, action->payload.send_toot.toot_id);
    return false;
  }
  remove_toot(store, action->payload.send_toot.toot_id);
  return true;
}

bool create_action_from_send_toot_result_message(int pid, crmna_buf_t *message,
                                                 action_t *action,
                                                 crmna_err_t *err) {
  send_toot_result_t payload;
  if (!deserialize_send_toot_result(message, &payload)) {
    return false;
  }
  action->type = SEND_TOOT_RESULT;
  action->payload.send_toot_result.pid = pid;
  action->payload.send_toot_result.device_id = payload.device_id;
  action->payload.send_toot_result.toot_id = payload.toot_id;
  action->payload.send_toot_result.result = payload.result;
  return true;
}
bool send_toot_result(store_t *store, action_t *action, crmna_err_t *err) {
  uint32_t device_id;
  if (get_device_id_from_toot(store, action->payload.create_toot_result.toot_id,
                              &device_id, err)) {
    return false;
  }
  int pid;
  if (get_device_pid_from_toot(
          store, action->payload.create_toot_result.toot_id, &pid, err)) {
    return false;
  }
  if (pid == action->payload.send_toot_result.pid &&
      device_id != action->payload.send_toot_result.device_id) {
    return false;
  }
  if (action->payload.send_toot_result.result != 0) {
    set_toot_sent(store, action->payload.send_toot_result.toot_id);
  } else {
    set_toot_failer(store, action->payload.send_toot_result.toot_id);
  }

  return true;
}
