// #include "cremona.h"
// #include "src/message.h"
// #include <cstdarg>
// #include <cstdlib>
// #include <gtest/gtest.h>
// #include <iostream>
// #include <stddef.h>

// class destroy_device_mock {
// public:
//   static cremona_device_t *create_device(char *name) {
//     return new cremona_device_t();
//   }
//   static bool destroy_device(cremona_device_t *device) { return true; }
//   static void cleanup_device(cremona_device_t *device) {
//     delete device;
//   }

//   static bool destroy_device_err(cremona_device_t *device) { return false; }

//   static int send_message(uint32_t pid, int type, char *buf, size_t buf_size)
//   {
//     return 0;
//   }

//   static uint32_t get_time() { return 2345; }
//   static uint32_t get_rand() { return 2345; }

//   static void device_manager_lock(cremona_device_manager_t *manager) {
//       std::cout << "device manager lock" << std::endl;
//   }
//   static void device_manager_unlock(cremona_device_manager_t *manager) {
//     std::cout << "device manager unlock" << std::endl;
//   }

//   static void log(cremona_error_level_t level, char *fmt, ...) {
//     va_list ap;
//     // 可変長引数を１個の変数にまとめる
//     va_start(ap, fmt);
//     // まとめられた変数で処理する
//     std::vprintf(fmt, ap);
//     va_end(ap);
//     ;
//   }

//   static void device_lock(cremona_device_t *device) {
//       std::cout << "device lock " << device->id << std::endl;
//   }
//   static void device_unlock(cremona_device_t *device) {
//     std::cout << "device unlock " << device->id << std::endl;
//   }
// };

// static void create_device_manager(cremona_device_manager_t *device_manager) {
//   device_manager->config.devicce_manager_callbacks.get_rand =
//       &destroy_device_mock::get_rand;
//   device_manager->config.devicce_manager_callbacks.get_time =
//       &destroy_device_mock::get_time;
//   device_manager->config.devicce_manager_callbacks.send_message =
//       &destroy_device_mock::send_message;
//   device_manager->config.devicce_manager_callbacks.lock =
//       &destroy_device_mock::device_manager_lock;
//   device_manager->config.devicce_manager_callbacks.unlock =
//       &destroy_device_mock::device_manager_unlock;
//   device_manager->config.devicce_manager_callbacks.log =
//       &destroy_device_mock::log;
//   device_manager->config.device_collbacks.create_device =
//       &destroy_device_mock::create_device;
//   device_manager->config.device_collbacks.destroy_device =
//       &destroy_device_mock::destroy_device;
//   device_manager->config.device_collbacks.cleanup_device =
//       &destroy_device_mock::cleanup_device;
//   device_manager->config.device_collbacks.lock =
//       &destroy_device_mock::device_lock;
//   device_manager->config.device_collbacks.unlock =
//       &destroy_device_mock::device_unlock;
// }

// TEST(device_manager, destroy_device) {
//   cremona_device_manager device_manager;
//   create_device_manager(&device_manager);

//   init_device_manager(&device_manager);
//   char *name = "test_device";
//   unsigned char create_message[] = {0x91, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
//                                     0x64, 0x65, 0x76, 0x69, 0x63, 0x65};
//   char error_msg[100];
//   crmna_err err = {.error_msg = error_msg,
//                      .error_msg_len = sizeof(error_msg)};
//   bool result =
//       reciveMessage(&device_manager, 100, CRMNA_CREATE_DEVICE,
//                     (char *)create_message, sizeof(create_message), &err);

//   unsigned char destroy_message[] = {0x91, 0xcf, 0x00, 0x00, 0x09,
//                                      0x29, 0x00, 0x00, 0x09, 0x29};

//   result =
//       reciveMessage(&device_manager, 100, CRMNA_DESTROY_DEVICE,
//                     (char *)destroy_message, sizeof(destroy_message), &err);
//   EXPECT_TRUE(result);

//   destroy_device_manager(&device_manager);
// }

// TEST(device_manager, destroy_device_not_fount_device) {
//   cremona_device_manager device_manager;
//   create_device_manager(&device_manager);

//   init_device_manager(&device_manager);
//   char *name = "test_device";
//   unsigned char create_message[] = {0x91, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
//                                     0x64, 0x65, 0x76, 0x69, 0x63, 0x65};
//   char error_msg[100];
//   crmna_err err = {.error_msg = error_msg,
//                      .error_msg_len = sizeof(error_msg)};
//   bool result = reciveMessage(&device_manager, 100, CRMNA_CREATE_DEVICE,
//                               (char *)create_message, sizeof(create_message),
//                               &err);

//   EXPECT_TRUE(result);
//   unsigned char destroy_message[] = {0x91, 0xcf, 0x00, 0x00, 0x09,
//                                      0x29, 0x00, 0x00, 0x09, 0x28};

//   result = reciveMessage(&device_manager, 100, CRMNA_DESTROY_DEVICE,
//                          (char *)destroy_message, sizeof(destroy_message),
//                          &err);
//   EXPECT_FALSE(result);
//   EXPECT_STREQ("Cannot find device of 10071698311464.", error_msg);
// }

// TEST(device_manager, destroy_device_different_pid) {
//   cremona_device_manager device_manager;
//   create_device_manager(&device_manager);

//   init_device_manager(&device_manager);
//   char *name = "test_device";
//   unsigned char create_message[] = {0x91, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
//                                     0x64, 0x65, 0x76, 0x69, 0x63, 0x65};
//   char error_msg[100];
//   crmna_err err = {.error_msg = error_msg,
//                      .error_msg_len = sizeof(error_msg)};
//   bool result = reciveMessage(&device_manager, 100, CRMNA_CREATE_DEVICE,
//                               (char *)create_message, sizeof(create_message),
//                               &err);

//   EXPECT_TRUE(result);
//   unsigned char destroy_message[] = {0x91, 0xcf, 0x00, 0x00, 0x09,
//                                      0x29, 0x00, 0x00, 0x09, 0x29};

//   result = reciveMessage(&device_manager, 50, CRMNA_DESTROY_DEVICE,
//                          (char *)destroy_message, sizeof(destroy_message),
//                          &err);
//   EXPECT_FALSE(result);
//   EXPECT_STREQ("Deffecent pid. id: 10071698311465 reqest: 50 target: 100",
//                error_msg);
// }
