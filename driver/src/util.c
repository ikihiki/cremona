#include "cremona_internal.h"


uint64_t create_id(cremona_device_manager_t *device_manager) {
  uint64_t time = device_manager->config.devicce_manager_callbacks.get_time();
  uint64_t rand = device_manager->config.devicce_manager_callbacks.get_rand();
  return (time << 32) + rand;
}