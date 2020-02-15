#include "cremona.h"
#include "src/message.h"
#include <cstdlib>
#include <gtest/gtest.h>

class create_device_mock {
public:
  static void *malloc(size_t size) { return std::malloc(size); }

  static void *malloc_err(size_t size) { return NULL; }

  const static char *test_context;
  static bool create_device(cremona_device_t *device) {
    device->context = (void *)test_context;
    return true;
  }

  static bool create_device_err(cremona_device_t *device) { return false; }

  static int send_message(uint32_t pid, int type, char *buf, size_t buf_size) {
    return 0;
  }

  static uint32_t get_time() { return 2345; }
  static uint32_t get_rand() { return 2345; }

  static void lock(cremona_device_manager_t *manager) {}
  static void unlock(cremona_device_manager_t *manager) {}
};

const char *create_device_mock::test_context = "test_context";

TEST(device_manager, create_device) {
  cremona_device_manager device_manager;
  device_manager.config.devicce_manager_callbacks.allocate =
      &create_device_mock::malloc;
  device_manager.config.devicce_manager_callbacks.get_rand =
      &create_device_mock::get_rand;
  device_manager.config.devicce_manager_callbacks.get_time =
      &create_device_mock::get_time;
  device_manager.config.device_collbacks.create_device =
      &create_device_mock::create_device;
  device_manager.config.devicce_manager_callbacks.send_message =
      &create_device_mock::send_message;
  device_manager.config.devicce_manager_callbacks.lock =
      &create_device_mock::lock;
  device_manager.config.devicce_manager_callbacks.unlock =
      &create_device_mock::unlock;

  initDeviceManager(&device_manager);
  char *name = "test_device";
  unsigned char message[] = {0x91, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65};

  bool result = reciveMessage(&device_manager, 100, CRMNA_CREATE_CONN,
                              (char *)message, sizeof(message));

  ASSERT_TRUE(result);
  khint_t k = kh_get(dm, device_manager.devices, 10071698311465);
  ASSERT_NE(kh_end(device_manager.devices), k);
  ASSERT_STREQ(name, kh_val(device_manager.devices, k)->name);
  ASSERT_EQ((void *)create_device_mock::test_context,
            kh_val(device_manager.devices, k)->context);
  ASSERT_EQ(100, kh_val(device_manager.devices, k)->pid);
  ASSERT_EQ(10071698311465, kh_val(device_manager.devices, k)->id);
}

TEST(device_manager, create_device_invalid_message) {
  cremona_device_manager device_manager;
  initDeviceManager(&device_manager);
  char *name = "test_device";
  unsigned char message[] = {0x90, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65};

  int result = reciveMessage(&device_manager, 100, CRMNA_CREATE_CONN,
                             (char *)message, sizeof(message));

  ASSERT_FALSE(result);
}

TEST(device_manager, create_device_fail_allocate_device) {
  cremona_device_manager device_manager;
  device_manager.config.devicce_manager_callbacks.allocate =
      &create_device_mock::malloc_err;
  initDeviceManager(&device_manager);
  char *name = "test_device";
  unsigned char message[] = {0x91, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65};

  int result = reciveMessage(&device_manager, 100, CRMNA_CREATE_CONN,
                             (char *)message, sizeof(message));

  ASSERT_FALSE(result);
}

TEST(device_manager, create_device_fail_create_callback) {
  cremona_device_manager device_manager;
  device_manager.config.devicce_manager_callbacks.allocate =
      &create_device_mock::malloc;
  device_manager.config.devicce_manager_callbacks.get_rand =
      &create_device_mock::get_rand;
  device_manager.config.devicce_manager_callbacks.get_time =
      &create_device_mock::get_time;
  device_manager.config.device_collbacks.create_device =
      &create_device_mock::create_device_err;
  initDeviceManager(&device_manager);
  char *name = "test_device";
  unsigned char message[] = {0x91, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65};

  int result = reciveMessage(&device_manager, 100, CRMNA_CREATE_CONN,
                             (char *)message, sizeof(message));

  ASSERT_FALSE(result);
}