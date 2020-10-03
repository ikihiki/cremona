#include "cremona.h"
#include "message.h"
#include <cstdarg>
#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>
#include <stddef.h>

// class add_text_mock {
// public:
//   static cremona_device_t *create_device(char *name) {
//     return new cremona_device_t();
//   }
//   static bool destroy_device(cremona_device_t *device) { return true; }
//   static void cleanup_device(cremona_device_t *device) { delete device; }

//   static bool destroy_device_err(cremona_device_t *device) { return false; }

//   static int send_message(uint32_t pid, int type, char *buf, size_t buf_size) {
//     std::cout << "---send message---" << std::endl;
//     std::cout << "pid: " << pid << std::endl;
//     std::cout << "type: " << type << std::endl;
//     std::cout << "data: " << std::endl;
//     for (int i = 0; i < buf_size; i++) {
//       int a = buf[i];
//       std::cout << std::hex << a;
//       std::cout << ", ";
//     }
//     std::cout << std::endl;
//     std::cout << "---send message---" << std::endl;

//     return 0;
//   }

//   static uint32_t get_time() { return 2345; }
//   static uint32_t get_rand() { return 2345; }

//   static void device_manager_lock(cremona_device_manager_t *manager) {
//     std::cout << "device manager lock" << std::endl;
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
//     std::cout << "device lock " << device->id << std::endl;
//   }
//   static void device_unlock(cremona_device_t *device) {
//     std::cout << "device unlock " << device->id << std::endl;
//   }

//   static cremona_toot_t *create_toot() { return new cremona_toot_t(); }

//   static void lock_toot(cremona_toot_t *toot) {
//     std::cout << "toot lock " << toot->id << std::endl;
//   }
//   static void unlock_toot(cremona_toot_t *toot) {
//     std::cout << "toot unlock " << toot->id << std::endl;
//   }
//   static void wait_toot(cremona_toot_t *toot, toot_wait_type_t wait_type) {
//     std::cout << "toot wait " << toot->id << "wait type" << wait_type
//               << std::endl;
//   }
//   static void notify_toot(cremona_toot_t *toot) {
//     std::cout << "toot notify " << toot->id << std::endl;
//   }
//   static bool destroy_toot(cremona_toot_t *toot) {
//     std::cout << "toot destroy " << toot->id << std::endl;
//     return true;
//   }

//   static void cleanup_toot(cremona_toot_t *toot) {
//     std::cout << "toot cleanup " << toot->id << std::endl;
//     delete toot;
//   }
// };

// static void create_device_manager(cremona_device_manager_t *device_manager) {
//   device_manager->config.devicce_manager_callbacks.get_rand =
//       &add_text_mock::get_rand;
//   device_manager->config.devicce_manager_callbacks.get_time =
//       &add_text_mock::get_time;
//   device_manager->config.devicce_manager_callbacks.send_message =
//       &add_text_mock::send_message;
//   device_manager->config.devicce_manager_callbacks.lock =
//       &add_text_mock::device_manager_lock;
//   device_manager->config.devicce_manager_callbacks.unlock =
//       &add_text_mock::device_manager_unlock;
//   device_manager->config.devicce_manager_callbacks.log = &add_text_mock::log;
//   device_manager->config.device_collbacks.create_device =
//       &add_text_mock::create_device;
//   device_manager->config.device_collbacks.destroy_device =
//       &add_text_mock::destroy_device;
//   device_manager->config.device_collbacks.cleanup_device =
//       &add_text_mock::cleanup_device;
//   device_manager->config.device_collbacks.lock = &add_text_mock::device_lock;
//   device_manager->config.device_collbacks.unlock =
//       &add_text_mock::device_unlock;

//   device_manager->config.toot_callbacks.create_toot =
//       &add_text_mock::create_toot;
//   device_manager->config.toot_callbacks.wait = &add_text_mock::wait_toot;

//   device_manager->config.toot_callbacks.notify = &add_text_mock::notify_toot;
//   device_manager->config.toot_callbacks.lock = &add_text_mock::lock_toot;
//   device_manager->config.toot_callbacks.unlock = &add_text_mock::unlock_toot;
//   device_manager->config.toot_callbacks.destroy_toot =
//       &add_text_mock::destroy_toot;
//   device_manager->config.toot_callbacks.cleanup_toot =
//       &add_text_mock::cleanup_toot;
// }

// TEST(toot, send_text) {
//   cremona_device_manager_t device_manager;
//   create_device_manager(&device_manager);

//   init_device_manager(&device_manager);
//   char *name = "test_device";
//   unsigned char create_message[] = {0x91, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
//                                     0x64, 0x65, 0x76, 0x69, 0x63, 0x65};
//   char error_msg[100];
//   crmna_err_t err = {.error_msg = error_msg,
//                      .error_msg_len = sizeof(error_msg)};
//   reciveMessage(&device_manager, 100, CRMNA_CREATE_DEVICE,
//                 (char *)create_message, sizeof(create_message), &err);

//   khint_t k = kh_get(dm, device_manager.devices, 10071698311465);
//   cremona_device_t *device = kh_val(device_manager.devices, k);
//   release_toot(open_toot(device, false, &err));

//   k = kh_get(toot, device->toots, 10071698311465);
//   cremona_toot_t *toot = add_ref_toot(kh_val(device->toots, k));

//   unsigned char new_toot_result_message[] = {
//       0x93, 0xcf, 0x00, 0x00, 0x09, 0x29, 0x00, 0x00, 0x09, 0x29, 0xcf, 0x00,
//       0x00, 0x09, 0x29, 0x00, 0x00, 0x09, 0x29, 0x00};
//   reciveMessage(&device_manager, 100, CRMNA_NEW_TOOT_RESULT,
//                 (char *)new_toot_result_message,
//                 sizeof(new_toot_result_message), &err);
//   char *text = "sample_text";
//   bool result = add_toot_text(toot, text, true, &err);

//   EXPECT_TRUE(result);
//   EXPECT_EQ(ADD_TEXT_RESULT_WAIT, toot->state);
//   EXPECT_EQ(strlen(text), toot->send_count);
//   unsigned char add_text_result_message[] = {
//       0x93, 0xcf, 0x00, 0x00, 0x09, 0x29, 0x00, 0x00, 0x09, 0x29, 0xcf, 0x00,
//       0x00, 0x09, 0x29, 0x00, 0x00, 0x09, 0x29, 0x0b};

//   reciveMessage(&device_manager, 100, CRMNA_ADD_TOOT_TEXT_RESULT,
//                 (char *)add_text_result_message,
//                 sizeof(add_text_result_message), &err);
//   EXPECT_EQ(WRITE_COMPLEATE, toot->state);

//   EXPECT_EQ(0, toot->send_count);
//   EXPECT_EQ(strlen(text), toot->prev_count);

//   release_toot(toot);

//   destroy_device_manager(&device_manager);
// }