#include "central_store.h"
#include "message.h"

bool create_action_from_create_device_message(int pid, crmna_buf_t *message,
                                              action_t *action,
                                              crmna_err_t *err) {
  create_device_t payload;
  if (!deserialize_create_conn(message, &payload)) {
    return false;
  }
  action->type = CREATE_DEVICE;
  action->payload.create_device.pid = pid;
  action->payload.create_device.uid = payload.uid;
  strncpy(action->payload.create_device.name, payload.name, 50);
  return true;
}

bool create_device(store_t *store, action_t *action, crmna_err_t *err) {
  uint32_t device_id;
  bool result = add_device(store, action->payload.create_device.pid,
                           action->payload.create_device.uid,
                           action->payload.create_device.name, &device_id, err);
  if (!result) {
    return false;
  }

  if (!attach_device_class(store, device_id, err)) {
    remove_device(store, device_id);
    return false;
  }

  create_device_result_t msg;
  msg.id = device_id;
  DEFINE_CRMNA_BUF(buf, 10)
  if (!serialize_create_device_result(&msg, &buf)) {
    detach_device_class(store, device_id);
    remove_device(store, device_id);
    return false;
  }

  communicator_ref_t com = get_communicator(store);
  if (communicator_send_message(&com, action->payload.create_device.pid,
                                CRMNA_CREATE_DEVICE_RESULT, &buf,
                                err) != buf.buf_size) {
    detach_device_class(store, device_id);
    remove_device(store, device_id);
    return false;
  }
  set_device_ready(store, device_id);
  return true;
}

bool create_action_from_destroy_device_message(int pid, crmna_buf_t *message,
                                               action_t *action,
                                               crmna_err_t *err) {
  destroy_device_t payload;
  if (!deserialize_destroy_device(message, &payload)) {
    return false;
  }
  action->type = DESTROY_DEVICE;
  action->payload.destroy_device.pid = pid;
  action->payload.destroy_device.device_id = payload.id;
  return true;
}
bool destroy_device(store_t *store, action_t *action, crmna_err_t *err) {
  uint32_t device_id = action->payload.destroy_device.device_id;
  destroy_device_result_t msg;
  msg.id = device_id;
  DEFINE_CRMNA_BUF(buf, 10)
  if (!serialize_destroy_device_result(&msg, &buf)) {
    detach_device_class(store, action->payload.destroy_device.device_id);
    remove_device(store, action->payload.destroy_device.device_id);
    return false;
  }

  communicator_ref_t com = get_communicator(store);
  if (communicator_send_message(&com, action->payload.create_device.pid,
                                CRMNA_CREATE_DEVICE_RESULT, &buf,
                                err) != buf.buf_size) {
    detach_device_class(store, action->payload.destroy_device.device_id);
    remove_device(store, action->payload.destroy_device.device_id);
    return false;
  }
  set_device_ready(store, device_id);
  return true;
}
