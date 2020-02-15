#include "cremona.h"
#include "src/message.h"
#include <cstdlib>
#include <gtest/gtest.h>
#include <stddef.h>

class destroy_device_mock {
public:
  static void *malloc(size_t size) { return std::malloc(size); }
  static void *malloc_err(size_t size) { return NULL; }

  static void free(void *ptr ) { std::free(ptr); }

  static bool create_device(cremona_device_t *device) { return true; }

  static bool destroy_device(cremona_device_t *device) { return true; }

  static bool destroy_device_err(cremona_device_t *device) {
    return false;
  }

  static int send_message(uint32_t pid, int type, char *buf, size_t buf_size) {
    return 0;
  }

  static uint32_t get_time() { return 2345; }
  static uint32_t get_rand() { return 2345; }

  static void lock(cremona_device_manager_t *manager) {}
  static void unlock(cremona_device_manager_t *manager) {}
};

TEST(device_manager, destroy_device) {
  cremona_device_manager device_manager;
  device_manager.config.devicce_manager_callbacks.allocate =
      &destroy_device_mock::malloc;
  device_manager.config.devicce_manager_callbacks.deallocate =
      &destroy_device_mock::free;
  device_manager.config.devicce_manager_callbacks.get_rand =
      &destroy_device_mock::get_rand;
  device_manager.config.devicce_manager_callbacks.get_time =
      &destroy_device_mock::get_time;
  device_manager.config.device_collbacks.create_device =
      &destroy_device_mock::create_device;
  device_manager.config.device_collbacks.destroy_device =
      &destroy_device_mock::destroy_device;
  device_manager.config.devicce_manager_callbacks.send_message =
      &destroy_device_mock::send_message;
  device_manager.config.devicce_manager_callbacks.lock =
      &destroy_device_mock::lock;
  device_manager.config.devicce_manager_callbacks.unlock =
      &destroy_device_mock::unlock;

  initDeviceManager(&device_manager);
  char *name = "test_device";
  unsigned char create_message[] = {0x91, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                                    0x64, 0x65, 0x76, 0x69, 0x63, 0x65};

  int result = reciveMessage(&device_manager, 100, CRMNA_CREATE_CONN,
                             (char *)create_message, sizeof(create_message));

  ASSERT_TRUE(result);
  unsigned char destroy_message[] = {0x91, 0xcf, 0x00, 0x00, 0x09,
                                     0x29, 0x00, 0x00, 0x09, 0x29};

  result = reciveMessage(&device_manager, 100, CRMNA_DESTROY_DEVICE,
                         (char *)destroy_message, sizeof(destroy_message));
  ASSERT_TRUE(result);
}
