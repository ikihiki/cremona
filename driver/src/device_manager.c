#ifdef CRMNA_TEST
#include <string.h>
#else
#include <linux/string.h>
#endif
#include "message.h"

#include "cremona.h"

void *cremona_malloc(cremona_device_manager_t *device_manager, int size) {
  return device_manager->config.devicce_manager_callbacks.allocate(size);
}

int send_message(cremona_device_manager_t *device_manager, uint32_t pid,
                 int type, char *buf, size_t buf_size) {
  device_manager->config.devicce_manager_callbacks.send_message(pid, type, buf,
                                                                buf_size);
}
int initDeviceManager(cremona_device_manager_t *device_manager) {
  device_manager->devices = kh_init(dm);
}

bool get_stats(cremona_device_manager_t *device_manager, uint32_t pid) {
  get_stats_result_t result;
  result.total_device_count = 0;
  khint_t k;
  
  device_manager->config.devicce_manager_callbacks.lock(device_manager);
  for (k = kh_begin(device_manager->devices);
       k != kh_end(device_manager->devices); ++k)
  {
    result.total_device_count++;
  }
  device_manager->config.devicce_manager_callbacks.unlock(device_manager);

  char message[100];
  size_t size = serialize_get_stats_result(&result, message, sizeof(message));
  send_message(device_manager, pid, CRMNA_GET_STATS_RESULT, message, size);
  return true;
}

int add_device(cremona_device_manager_t *device_manager,
               cremona_device_t *device) {
  int ret, is_missing;
  khiter_t k;
  k = kh_put(dm, device_manager->devices, device->id, &ret);
  kh_value(device_manager->devices, k) = device;
}

uint64_t create_id(cremona_device_manager_t *device_manager) {
  uint64_t time = device_manager->config.devicce_manager_callbacks.get_time();
  uint64_t rand = device_manager->config.devicce_manager_callbacks.get_rand();
  return (time << 32) + rand;
}

bool create_device(cremona_device_manager_t *device_manager, uint32_t pid,
                   char *buf, size_t buf_size) {
  create_conn data;
  if (!deserialize_create_conn(buf, buf_size, &data)) {
    return false;
  }
  cremona_device_t *device = (cremona_device_t *)cremona_malloc(
      device_manager, sizeof(cremona_device_t));
  if (!device) {
    return false;
  }
  device->id = create_id(device_manager);
  device->pid = pid;
  memcpy(device->name, data.name, sizeof(device->name));
  if (!device_manager->config.device_collbacks.create_device(device)) {
    return false;
  }
  add_device(device_manager, device);
  create_conn_result result = {device->id, true, NULL};
  char message[100];
  serialize_create_conn_result(&result, message, sizeof(message));
  send_message(device_manager, pid, 0, message, strlen(message));
  return true;
}

bool destroy_device(cremona_device_manager_t *device_manager, uint32_t pid,
                    char *buf, size_t buf_size) {
  destroy_device_t msg;
  deserialize_destroy_device(buf, buf_size, &msg);
  khint_t k = kh_get(dm, device_manager->devices, msg.id);
  if (k == kh_end(device_manager->devices)) {
    return false;
  }
  cremona_device_t *device = kh_val(device_manager->devices, k);
  device_manager->config.device_collbacks.destroy_device(device);
  kh_del(dm, device_manager->devices, k);
  device_manager->config.devicce_manager_callbacks.deallocate(device);
  return true;
}

bool reciveMessage(cremona_device_manager_t *device_manager, uint32_t pid, int type,
                   char *buf, size_t buf_size) {
  switch (type) {
  case CRMNA_GET_STATS:
    return get_stats(device_manager, pid);
  case CRMNA_CREATE_CONN:
    return create_device(device_manager, pid, buf, buf_size);
  case CRMNA_DESTROY_DEVICE:
    return destroy_device(device_manager, pid, buf, buf_size);

  default:
    return false;
  }
}