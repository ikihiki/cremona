#include "cremona.h"
#include "src/message.h"
#include <cstdarg>
#include <cstdlib>
#include <gtest/gtest.h>

class create_device_mock {
public:
  const static char *test_context;
  static cremona_device_t *create_device(char *name) {
    return new cremona_device_t();
  }
  static bool destroy_device(cremona_device_t *device) { return true; }
  static void cleanup_device(cremona_device_t *device) { delete device; }

  static cremona_device_t *create_device_err(char *name) { return NULL; }

  static int send_message(uint32_t pid, int type, char *buf, size_t buf_size) {
    return 0;
  }

  static uint32_t get_time() { return 2345; }
  static uint32_t get_rand() { return 2345; }
  static void log(cremona_error_level_t level, char *fmt, ...) {
    va_list ap;
    // 可変長引数を１個の変数にまとめる
    va_start(ap, fmt);
    // まとめられた変数で処理する
    std::vprintf(fmt, ap);
    va_end(ap);
    ;
  }

  static void lock(cremona_device_manager_t *manager) {}
  static void unlock(cremona_device_manager_t *manager) {}

  static void device_lock(cremona_device_t *device) {
    std::cout << "device lock " << device->id << std::endl;
  }
  static void device_unlock(cremona_device_t *device) {
    std::cout << "device unlock " << device->id << std::endl;
  }
};

const char *create_device_mock::test_context = "test_context";

static void create_device_manager(cremona_device_manager_t *device_manager) {
  device_manager->config.devicce_manager_callbacks.get_rand =
      &create_device_mock::get_rand;
  device_manager->config.devicce_manager_callbacks.get_time =
      &create_device_mock::get_time;
  device_manager->config.device_collbacks.create_device =
      &create_device_mock::create_device;
  device_manager->config.device_collbacks.destroy_device =
      &create_device_mock::destroy_device;
  device_manager->config.device_collbacks.cleanup_device =
      &create_device_mock::cleanup_device;
  device_manager->config.devicce_manager_callbacks.send_message =
      &create_device_mock::send_message;
  device_manager->config.devicce_manager_callbacks.lock =
      &create_device_mock::lock;
  device_manager->config.devicce_manager_callbacks.unlock =
      &create_device_mock::unlock;
  device_manager->config.devicce_manager_callbacks.log =
      &create_device_mock::log;

  device_manager->config.device_collbacks.lock =
      &create_device_mock::device_lock;
  device_manager->config.device_collbacks.unlock =
      &create_device_mock::device_unlock;
}

TEST(device_manager, create_device) {
  cremona_device_manager_t device_manager;
  create_device_manager(&device_manager);
  device_manager.devices = nullptr;
  init_device_manager(&device_manager);
  char *name = "test_device";
  unsigned char message[] = {0x91, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65};

  char error_msg[100];
  crmna_err_t err = {.error_msg = error_msg,
                     .error_msg_len = sizeof(error_msg)};
  bool result = reciveMessage(&device_manager, 100, CRMNA_CREATE_DEVICE,
                              (char *)message, sizeof(message), &err);

  EXPECT_TRUE(result);
  khint_t k = kh_get(dm, device_manager.devices, 10071698311465);
  EXPECT_NE(kh_end(device_manager.devices), k);
  cremona_device_t *device = kh_val(device_manager.devices, k);
  EXPECT_STREQ(name, device->name);
  EXPECT_EQ(100, device->pid);
  EXPECT_EQ(10071698311465, device->id);
  EXPECT_EQ(1, device->refCount);
  EXPECT_FALSE(device->isDestroied);

  destroy_device_manager(&device_manager);
}

TEST(device_manager, create_device_invalid_message) {
  cremona_device_manager device_manager;
  create_device_manager(&device_manager);
  init_device_manager(&device_manager);
  char *name = "test_device";
  unsigned char message[] = {0x90, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65};
  char error_msg[100];
  crmna_err_t err = {.error_msg = error_msg,
                     .error_msg_len = sizeof(error_msg)};

  int result = reciveMessage(&device_manager, 100, CRMNA_CREATE_DEVICE,
                             (char *)message, sizeof(message), &err);

  EXPECT_FALSE(result);
  EXPECT_STREQ("Cannot decode create device message. pid: 100", error_msg);
  destroy_device_manager(&device_manager);
}

TEST(device_manager, create_device_fail_create_callback) {
  cremona_device_manager device_manager;
  create_device_manager(&device_manager);
  device_manager.config.device_collbacks.create_device =
      &create_device_mock::create_device_err;
  init_device_manager(&device_manager);
  char *name = "test_device";
  unsigned char message[] = {0x91, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65};
  char error_msg[100];
  crmna_err_t err = {.error_msg = error_msg,
                     .error_msg_len = sizeof(error_msg)};

  int result = reciveMessage(&device_manager, 100, CRMNA_CREATE_DEVICE,
                             (char *)message, sizeof(message), &err);

  EXPECT_FALSE(result);
  EXPECT_STREQ("Cannot create device. pid: 100", error_msg);
  destroy_device_manager(&device_manager);
}