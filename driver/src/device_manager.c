#include "message.h"

#include "cremona_internal.h"

void cremona_device_manager_lock(cremona_device_manager_t *device_manager) {
  // LOG_ERROR(device_manager, " lock device manager %p", device_manager);

  // device_manager->config.devicce_manager_callbacks.lock(device_manager);
}
void cremona_device_manager_unlock(cremona_device_manager_t *device_manager) {
  // LOG_ERROR(device_manager, " unlock device manager %p", device_manager);

  // device_manager->config.devicce_manager_callbacks.unlock(device_manager);
}

cremona_device_manager_t *
add_ref_device_manager(cremona_device_manager_t *device_manager) {
  device_manager->refCount++;
  return device_manager;
}

void release_device_manager(cremona_device_manager_t *device_manager) {
  device_manager->refCount--;
}

int send_message(cremona_device_manager_t *device_manager, uint32_t pid,
                 int type, char *buf, size_t buf_size) {
//   return device_manager->config.devicce_manager_callbacks.send_message(
//       device_manager, pid, type, buf, buf_size);
// }
// bool init_device_manager(cremona_device_manager_t *device_manager) {
//   device_manager->devices = kh_init(dm);
//   device_manager->refCount = 0;
  return true;
}

bool rent_miner_num(cremona_device_manager_t *device_manager,
                    unsigned int *miner) {
  // cremona_device_manager_lock(device_manager);
  // unsigned int result;
  // LOG_ERROR(device_manager, "rent_miner_num %p", miner);

  // if (device_manager->config.devicce_manager_callbacks.rent_miner_num(
  //         device_manager, &result)) {
  //   LOG_ERROR(device_manager, "rent_miner_num %p %d", miner, result);
  //   *miner = result;
  //   cremona_device_manager_unlock(device_manager);
  //   return true;
  // } else {
  //   LOG_ERROR(device_manager, "rent_miner_num %p %d", miner, result);
  //   *miner = 0;
  //   cremona_device_manager_unlock(device_manager);
  //   return false;
  // }

  // cremona_device_manager_unlock(device_manager);
}
void release_miner_num(cremona_device_manager_t *device_manager,
                       unsigned int miner) {
  // cremona_device_manager_lock(device_manager);

  // device_manager->config.devicce_manager_callbacks.release_miner_num(
  //     device_manager, miner);

  // cremona_device_manager_unlock(device_manager);
}

void destroy_device_manager(cremona_device_manager_t *device_manager) {
  // khint_t k;
  // char error_msg[100];
  // crmna_err_t err = {.error_msg = error_msg,
  //                    .error_msg_len = sizeof(error_msg)};

  // device_manager->config.devicce_manager_callbacks.lock(device_manager);
  // for (k = kh_begin(device_manager->devices);
  //      k != kh_end(device_manager->devices); ++k) {
  //   if (kh_exist(device_manager->devices, k)) {
  //     if (!destroy_device(kh_value(device_manager->devices, k), &err)) {
  //       LOG_ERROR(device_manager, "Destroy Error error: %s", error_msg);
  //     }
  //   }
  // }
  // device_manager->config.devicce_manager_callbacks.unlock(device_manager);

  // kh_destroy(dm, device_manager->devices);
}

bool get_stats(cremona_device_manager_t *device_manager, uint32_t pid) {
  // get_stats_result_t result;
  // result.total_device_count = 0;
  // khint_t k;

  // device_manager->config.devicce_manager_callbacks.lock(device_manager);
  // for (k = kh_begin(device_manager->devices);
  //      k != kh_end(device_manager->devices); ++k) {
  //   if (kh_exist(device_manager->devices, k)) {
  //     result.total_device_count++;
  //   }
  // }
  // device_manager->config.devicce_manager_callbacks.unlock(device_manager);

  // char message[100];
  // size_t size = 11;
  // //serialize_get_stats_result(&result, message, sizeof(message));
  // send_message(device_manager, pid, CRMNA_GET_STATS_RESULT, message, size);
  return true;
}

static bool create_device_message(cremona_device_manager_t *device_manager,
                                  uint32_t pid, char *buf, size_t buf_size,
                                  crmna_err_t *error) {
  // create_device_t data;
  // if (false/*!deserialize_create_conn(buf, buf_size, &data)*/) {
  //   LOG_AND_WRITE_ERROR(device_manager, error,
  //                       "Cannot decode create device message. pid: %d", pid);
  //   return false;
  // }

  // cremona_device_t *device = NULL;
  // // create_device(create_id(device_manager), pid, data.uid, data.name,
  // //               device_manager, error);
  // if (!device) {
  //   LOG_AND_WRITE_ERROR(device_manager, error, "Cannot create device. pid: %d",
  //                       pid);
  //   return false;
  // }

  // add_device(device_manager, device);
  // create_device_result_t result_message = {device->id};
  // char message[100];
  // int msg_size = 10;
  // //serialize_create_device_result(&result_message, message, sizeof(message));
  // send_message(device_manager, pid, CRMNA_CREATE_DEVICE_RESULT, message,
  //              msg_size);
  // LOG_INFO(device_manager, "Device created. name: %s id: %ld", device->name,
  //          device->id);
  return true;
}

bool destroy_device_message(cremona_device_manager_t *device_manager,
                            uint32_t pid, char *buf, size_t buf_size,
                            crmna_err_t *error) {
  // destroy_device_t msg;
  // if (false /*!deserialize_destroy_device(buf, buf_size, &msg)*/) {
  //   LOG_AND_WRITE_ERROR(device_manager, error,
  //                       "Cannot deserialize message. pid: %d", pid);
  //   return false;
  // }

  // cremona_device_t *device = cremna_get_device(device_manager, msg.id);
  // if (device == NULL) {
  //   LOG_AND_WRITE_ERROR(device_manager, error, "Cannot find device of %llu.",
  //                       msg.id);
  //   return false;
  // }
  // LOG_ERROR(device_manager, "%p %p %d %d %d", device_manager,
  //           device->device_manager, device->id, device->pid, device->refCount);

  // cremona_device_lock(device);
  // uint64_t deviceId = device->id;
  // uint32_t devicePid = device->pid;
  // cremona_device_unlock(device);
  // if (devicePid != pid) {
  //   // LOG_AND_WRITE_ERROR(device_manager, error,
  //   //                     "Deffecent pid. id: %llu reqest: %d target: %d",
  //   //                     deviceId, pid, devicePid);
  //   return false;
  // }

  // // destroy_device(device, error);
  // delete_device(device_manager, msg.id);
  // release_device(device);

  // destroy_device_result_t result = {deviceId};
  // char message[100];
  // int msg_size = 10;
  // //serialize_destroy_device_result(&result, message, sizeof(message));
  // send_message(device_manager, pid, CRMNA_DESTROY_DEVICE_RESULT, message,
  //              msg_size);
  return true;
}

bool new_toot_result_message(cremona_device_manager_t *device_manager,
                             uint32_t pid, char *buf, size_t buf_size,
                             crmna_err_t *error) {
//   new_toot_result_t msg;
//   bool result = true;
//   cremona_device_t *device = NULL;
//   cremona_toot_t *toot = NULL;

//   if (false /*!deserialize_new_toot_result(buf, buf_size, &msg)*/) {
//     LOG_AND_WRITE_ERROR(device_manager, error,
//                         "Cannot deserialize message. pid: %d", pid);
//     result = false;
//     goto end;
//   }

//   LOG_ERROR(device_manager, "Recive msg  device id: %llu toot id: %llu",
//             msg.device_id, msg.toot_id);

//   device = cremna_get_device(device_manager, msg.device_id);
//   if (device == NULL) {
//     LOG_AND_WRITE_ERROR(device_manager, error, "Cannot find device. id: %llu",
//                         msg.device_id);
//     result = false;
//     goto end;
//   }

//   toot = cremna_get_toot(device, msg.toot_id);
//   if (toot == NULL) {
//     LOG_AND_WRITE_ERROR(device_manager, error, "Cannot find toot. id: %llu",
//                         msg.toot_id);
//     result = false;
//     goto end;
//   }

//   if (!recive_open_toot_result(toot, &msg, error)) {
//     LOG_AND_WRITE_ERROR(device_manager, error,
//                         "Failed open toot. device id: %llu toot id: %llu",
//                         msg.device_id, msg.toot_id);
//     result = false;
//     goto end;
//   }

// end:
//   release_toot(toot);
//   release_device(device);

//   return result;
}

bool send_toot_result_message(cremona_device_manager_t *device_manager,
                              uint32_t pid, char *buf, size_t buf_size,
                              crmna_err_t *error) {
//   send_toot_result_t msg;
//   bool result = true;
//   cremona_device_t *device = NULL;
//   cremona_toot_t *toot = NULL;

//   if (false /*!deserialize_send_toot_result(buf, buf_size, &msg)*/) {
//     LOG_AND_WRITE_ERROR(device_manager, error,
//                         "Cannot deserialize message. pid: %d", pid);
//     result = false;
//     goto end;
//   }

//   device = cremna_get_device(device_manager, msg.device_id);
//   if (device == NULL) {
//     LOG_AND_WRITE_ERROR(device_manager, error, "Cannot find device. id: %llu",
//                         msg.device_id);
//     result = false;
//     goto end;
//   }

//   toot = cremna_get_toot(device, msg.toot_id);
//   if (toot == NULL) {
//     LOG_AND_WRITE_ERROR(device_manager, error, "Cannot find toot. id: %llu",
//                         msg.toot_id);
//     result = false;
//     goto end;
//   }

//   if (!recive_close_toot_result(toot, device, &msg, error)) {
//     LOG_AND_WRITE_ERROR(device_manager, error,
//                         "Failed close toot. device id: %llu toot id: %llu",
//                         msg.device_id, msg.toot_id);
//     result = false;
//     goto end;
//   }

// end:
//   release_toot(toot);
//   release_device(device);

//   return result;
}

bool add_toot_text_result_message(cremona_device_manager_t *device_manager,
                                  uint32_t pid, char *buf, size_t buf_size,
                                  crmna_err_t *error) {
//   add_toot_text_result_t msg;
//   bool result = true;
//   cremona_device_t *device = NULL;
//   cremona_toot_t *toot = NULL;

//   if (false /*!deserialize_add_toot_text_result(buf, buf_size, &msg)*/) {
//     LOG_AND_WRITE_ERROR(device_manager, error,
//                         "Cannot deserialize message. pid: %d", pid);
//     result = false;
//     goto end;
//   }

//   device = cremna_get_device(device_manager, msg.device_id);
//   if (device == NULL) {
//     LOG_AND_WRITE_ERROR(device_manager, error, "Cannot find device. id: %llu",
//                         msg.device_id);
//     result = false;
//     goto end;
//   }

//   toot = cremna_get_toot(device, msg.toot_id);
//   if (toot == NULL) {
//     LOG_AND_WRITE_ERROR(device_manager, error, "Cannot find toot. id: %llu",
//                         msg.toot_id);
//     result = false;
//     goto end;
//   }

//   if (!recive_add_toot_text_result(toot, &msg, error)) {
//     LOG_AND_WRITE_ERROR(device_manager, error,
//                         "Failed add toot text. device id: %llu toot id: %llu",
//                         msg.device_id, msg.toot_id);
//     result = false;
//     goto end;
//   }

// end:
//   release_toot(toot);
//   release_device(device);

//   return result;
}

bool reciveMessage(cremona_device_manager_t *device_manager, uint32_t pid,
                   int type, char *buf, size_t buf_size, crmna_err_t *error) {
  // switch (type) {
  // case CRMNA_GET_STATS:
  //   return get_stats(device_manager, pid);
  // case CRMNA_CREATE_DEVICE:
  //   return create_device_message(device_manager, pid, buf, buf_size, error);
  // case CRMNA_DESTROY_DEVICE:
  //   return destroy_device_message(device_manager, pid, buf, buf_size, error);
  // case CRMNA_NEW_TOOT_RESULT:
  //   return new_toot_result_message(device_manager, pid, buf, buf_size, error);
  // case CRMNA_SEND_TOOT_RESULT:
  //   return send_toot_result_message(device_manager, pid, buf, buf_size, error);
  // case CRMNA_ADD_TOOT_TEXT_RESULT:
  //   return add_toot_text_result_message(device_manager, pid, buf, buf_size,
  //                                       error);

  // default:
  //   LOG_AND_WRITE_ERROR(device_manager, error,
  //                       "Unknown message type. type: %d pid: %u", type, pid);
  //   return false;
  // }
}