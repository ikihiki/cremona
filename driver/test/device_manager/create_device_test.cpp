#include "../mock/test_allocater.h"
#include "../mock/test_communicator.h"
#include "../mock/test_id_mapper.h"
#include "../mock/test_locker.h"
#include "../mock/test_logger.h"
#include "cremona.h"
#include "src/message.h"
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

// class create_device_mock {
// public:
//   const static char *test_context;
//   static cremona_device_t *create_device(char *name) {
//     return new cremona_device_t();
//   }
//   static bool destroy_device(cremona_device_t *device) { return true; }
//   static void cleanup_device(cremona_device_t *device) { delete device; }

//   static cremona_device_t *create_device_err(char *name) { return NULL; }

//   static int send_message(uint32_t pid, int type, char *buf, size_t buf_size)
//   {
//     return 0;
//   }

//   static uint32_t get_time() { return 2345; }
//   static uint32_t get_rand() { return 2345; }
//   static void log(cremona_error_level_t level, char *fmt, ...) {
//     va_list ap;
//     // 可変長引数を１個の変数にまとめる
//     va_start(ap, fmt);
//     // まとめられた変数で処理する
//     std::vprintf(fmt, ap);
//     va_end(ap);
//     ;
//   }

//   static void lock(cremona_device_manager_t *manager) {}
//   static void unlock(cremona_device_manager_t *manager) {}

//   static void device_lock(cremona_device_t *device) {
//     std::cout << "device lock " << device->id << std::endl;
//   }
//   static void device_unlock(cremona_device_t *device) {
//     std::cout << "device unlock " << device->id << std::endl;
//   }
// };

// const char *create_device_mock::test_context = "test_context";

// static void create_device_manager(cremona_device_manager_t *device_manager) {
//   device_manager->config.devicce_manager_callbacks.get_rand =
//       &create_device_mock::get_rand;
//   device_manager->config.devicce_manager_callbacks.get_time =
//       &create_device_mock::get_time;
//   device_manager->config.device_collbacks.create_device =
//       &create_device_mock::create_device;
//   device_manager->config.device_collbacks.destroy_device =
//       &create_device_mock::destroy_device;
//   device_manager->config.device_collbacks.cleanup_device =
//       &create_device_mock::cleanup_device;
//   device_manager->config.devicce_manager_callbacks.send_message =
//       &create_device_mock::send_message;
//   device_manager->config.devicce_manager_callbacks.lock =
//       &create_device_mock::lock;
//   device_manager->config.devicce_manager_callbacks.unlock =
//       &create_device_mock::unlock;
//   device_manager->config.devicce_manager_callbacks.log =
//       &create_device_mock::log;

//   device_manager->config.device_collbacks.lock =
//       &create_device_mock::device_lock;
//   device_manager->config.device_collbacks.unlock =
//       &create_device_mock::device_unlock;
// }

// TEST(device_manager, create_device) {
//   cremona_device_manager_t device_manager;
//   create_device_manager(&device_manager);
//   device_manager.devices = nullptr;
//   init_device_manager(&device_manager);
//   char *name = "test_device";
//   unsigned char message[] = {0x91, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
//                              0x64, 0x65, 0x76, 0x69, 0x63, 0x65};

//   char error_msg[100];
//   crmna_err_t err = {.error_msg = error_msg,
//                      .error_msg_len = sizeof(error_msg)};
//   bool result = reciveMessage(&device_manager, 100, CRMNA_CREATE_DEVICE,
//                               (char *)message, sizeof(message), &err);

//   EXPECT_TRUE(result);
//   khint_t k = kh_get(dm, device_manager.devices, 10071698311465);
//   EXPECT_NE(kh_end(device_manager.devices), k);
//   cremona_device_t *device = kh_val(device_manager.devices, k);
//   EXPECT_STREQ(name, device->name);
//   EXPECT_EQ(100, device->pid);
//   EXPECT_EQ(10071698311465, device->id);
//   EXPECT_EQ(1, device->refCount);
//   EXPECT_FALSE(device->isDestroied);

//   destroy_device_manager(&device_manager);
// }

// TEST(device_manager, create_device_invalid_message) {
//   cremona_device_manager device_manager;
//   create_device_manager(&device_manager);
//   init_device_manager(&device_manager);
//   char *name = "test_device";
//   unsigned char message[] = {0x90, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
//                              0x64, 0x65, 0x76, 0x69, 0x63, 0x65};
//   char error_msg[100];
//   crmna_err_t err = {.error_msg = error_msg,
//                      .error_msg_len = sizeof(error_msg)};

//   int result = reciveMessage(&device_manager, 100, CRMNA_CREATE_DEVICE,
//                              (char *)message, sizeof(message), &err);

//   EXPECT_FALSE(result);
//   EXPECT_STREQ("Cannot decode create device message. pid: 100", error_msg);
//   destroy_device_manager(&device_manager);
// }
using ::testing::_;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Pointee;
using ::testing::Return;

TEST(device_manager, create_device_message_invalid_message) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};

  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock, create_communicator(_, _, _))
      .WillOnce(Return(true));
  auto com_factory = com_factory_mock.get_factory();

  test_locker_mock locker_mock;
  test_locker_factory_mock locker_factory_mock;
  EXPECT_CALL(locker_factory_mock, create_locker(_, _)).WillOnce(Return(true));
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_factory_mock id_mock;
  EXPECT_CALL(id_mock, create_id_mapper(_, _, _, _)).WillOnce(Return(true));
  auto id_mapper_factory = id_mock.get_factory();

  bool result = init_device_manager(&device_manager, 0, 0, &com_factory,
                                    &locker_factory, &id_mapper_factory, NULL,
                                    NULL, &test_logger::logger_ref, NULL, &err);
  EXPECT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  EXPECT_STRNE(err_msg, "");
}

TEST(device_manager, create_device_message_fail_allocation) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};

  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock, create_communicator(_, _, _))
      .WillOnce(Return(true));
  auto com_factory = com_factory_mock.get_factory();

  test_locker_mock locker_mock;
  test_locker_factory_mock locker_factory_mock;
  EXPECT_CALL(locker_factory_mock, create_locker(_, _)).WillOnce(Return(true));
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_factory_mock id_mock;
  EXPECT_CALL(id_mock, create_id_mapper(_, _, _, _)).WillOnce(Return(true));
  auto id_mapper_factory = id_mock.get_factory();

  test_allocater_mock alloc_mock;
  EXPECT_CALL(alloc_mock, allocate(_)).WillOnce(Return((void *)NULL));
  auto alloc = alloc_mock.get_ref();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, NULL, &test_logger::logger_ref, &alloc, &err);
  EXPECT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  EXPECT_STRNE(err_msg, "");
}

TEST(device_manager, create_device_message_fail_add_device) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};

  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock, create_communicator(_, _, _))
      .WillOnce(Return(true));
  auto com_factory = com_factory_mock.get_factory();

  test_locker_mock locker_mock;
  test_locker_factory_mock locker_factory_mock;
  EXPECT_CALL(locker_factory_mock, create_locker(_, _)).WillOnce(Return(true));
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_mock id_mock;
  EXPECT_CALL(id_mock, add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *, crmna_err_t *err) {
                        snprintf(err->error_msg, err->error_msg_len,
                                 "error_add_device");
                      }),
                      Return(false)));

  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock, create_id_mapper(_, _, _, _))
      .WillOnce(
          DoAll(Invoke([&id_mock](id_mapper_ref *ref, int, int, crmna_err_t *) {
                  id_mock.set_ref(ref);
                }),
                Return(true)));
  auto id_mapper_factory = id_factory_mock.get_factory();

  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return((void *)&device));
  EXPECT_CALL(alloc_mock, free(Eq(&device))).WillOnce(Return());
  auto alloc = alloc_mock.get_ref();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, NULL, &test_logger::logger_ref, &alloc, &err);
  EXPECT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  EXPECT_STRNE(err_msg, "");
}

TEST(device_manager, create_device_message_fail_create_id_mapper) {

  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock, create_communicator(_, _, _))
      .WillOnce(

          Return(true));
  auto com_factory = com_factory_mock.get_factory();

  test_locker_mock locker_mock;
  test_locker_factory_mock locker_factory_mock;
  EXPECT_CALL(locker_factory_mock, create_locker(_, _))
      .WillOnce(Return(true))
      .WillOnce(Return(true));
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_mock id_mock;
  EXPECT_CALL(id_mock, add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *id, crmna_err_t *) { *id = 1; }),
                      Return(true)));
  EXPECT_CALL(id_mock, remove(1, _)).WillOnce(Return(true));

  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock, create_id_mapper(_, _, _, _))
      .WillOnce(
          DoAll(Invoke([&id_mock](id_mapper_ref *ref, int, int, crmna_err_t *) {
                  id_mock.set_ref(ref);
                }),
                Return(true)))
      .WillOnce(DoAll(Invoke([](id_mapper_ref *, int, int, crmna_err_t *err) {
                        snprintf(err->error_msg, err->error_msg_len,
                                 "error_id_mapper");
                      }),
                      Return(false)));
  auto id_mapper_factory = id_factory_mock.get_factory();

  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return((void *)&device));
  EXPECT_CALL(alloc_mock, free(Eq(&device))).WillOnce(Return());
  auto alloc = alloc_mock.get_ref();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, NULL, &test_logger::logger_ref, &alloc, &err);
  ASSERT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  // EXPECT_STREQ(err_msg, "error_id_mapper");
}

TEST(device_manager, create_device_message_fail_create_locker) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock, create_communicator(_, _, _))
      .WillOnce(

          Return(true));
  auto com_factory = com_factory_mock.get_factory();

  test_locker_mock locker_mock;
  test_locker_factory_mock locker_factory_mock;
  EXPECT_CALL(locker_factory_mock, create_locker(_, _))
      .WillOnce(Return(true))
      .WillOnce(DoAll(Invoke([](locker_ref *, crmna_err_t *err) {
                        snprintf(err->error_msg, err->error_msg_len,
                                 "err_create_locker");
                      }),
                      Return(false)));
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_mock id_mock;
  EXPECT_CALL(id_mock, add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *id, crmna_err_t *) { *id = 1; }),
                      Return(true)));
  EXPECT_CALL(id_mock, remove(1, _)).WillOnce(Return(true));

  test_id_mapper_mock toots_mock;
  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock, create_id_mapper(_, _, _, _))
      .WillOnce(
          DoAll(Invoke([&id_mock](id_mapper_ref *ref, int, int, crmna_err_t *) {
                  id_mock.set_ref(ref);
                }),
                Return(true)))
      .WillOnce(DoAll(
          Invoke([&toots_mock](id_mapper_ref *ref, int, int, crmna_err_t *) {
            toots_mock.set_ref(ref);
          }),
          Return(true)));
  auto id_mapper_factory = id_factory_mock.get_factory();

  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return(&device));
  EXPECT_CALL(alloc_mock, free(_));
  auto alloc = alloc_mock.get_ref();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, NULL, &test_logger::logger_ref, &alloc, &err);
  ASSERT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(&device_manager, 0, CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  // EXPECT_STREQ(err_msg, "err_create_locker");
}

TEST(device_manager, create_device_message_fail_send_compleate_message) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_mock com_mock;
  EXPECT_CALL(com_mock, send_message(10, CRMNA_CREATE_DEVICE_RESULT, _, _))
      .WillOnce(Return(-1));

  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock, create_communicator(_, _, _))
      .WillOnce(DoAll(
          Invoke([&com_mock](cremona_device_manager_t *, communicator_ref *ref,
                             crmna_err_t *) { com_mock.set_ref(ref); }),
          Return(true)));
  auto com_factory = com_factory_mock.get_factory();

  test_locker_mock locker_mock;
  test_locker_factory_mock locker_factory_mock;
  EXPECT_CALL(locker_factory_mock, create_locker(_, _))
      .WillOnce(Return(true))
      .WillOnce(Return(true));
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_mock id_mock;
  EXPECT_CALL(id_mock, add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *id, crmna_err_t *) { *id = 1; }),
                      Return(true)));
  EXPECT_CALL(id_mock, remove(1, _)).WillOnce(Return(true));

  test_id_mapper_mock toots_mock;
  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock, create_id_mapper(_, _, _, _))
      .WillOnce(
          DoAll(Invoke([&id_mock](id_mapper_ref *ref, int, int, crmna_err_t *) {
                  id_mock.set_ref(ref);
                }),
                Return(true)))
      .WillOnce(DoAll(
          Invoke([&toots_mock](id_mapper_ref *ref, int, int, crmna_err_t *) {
            toots_mock.set_ref(ref);
          }),
          Return(true)));
  auto id_mapper_factory = id_factory_mock.get_factory();

  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return(&device));
  EXPECT_CALL(alloc_mock, free(_));
  auto alloc = alloc_mock.get_ref();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, NULL, &test_logger::logger_ref, &alloc, &err);
  ASSERT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  // EXPECT_STREQ(err_msg, "err_create_locker");
}

TEST(device_manager, create_device_message_success) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_mock com_mock;
  EXPECT_CALL(com_mock, send_message(10, CRMNA_CREATE_DEVICE_RESULT, _, _))
      .WillOnce(Return(10));

  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock, create_communicator(_, _, _))
      .WillOnce(DoAll(
          Invoke([&com_mock](cremona_device_manager_t *, communicator_ref *ref,
                             crmna_err_t *) { com_mock.set_ref(ref); }),
          Return(true)));
  auto com_factory = com_factory_mock.get_factory();

  test_locker_mock locker_mock;
  test_locker_factory_mock locker_factory_mock;
  EXPECT_CALL(locker_factory_mock, create_locker(_, _))
      .WillOnce(Return(true))
      .WillOnce(Return(true));
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_mock id_mock;
  EXPECT_CALL(id_mock, add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *id, crmna_err_t *) { *id = 1; }),
                      Return(true)));

  test_id_mapper_mock toots_mock;
  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock, create_id_mapper(_, _, _, _))
      .WillOnce(
          DoAll(Invoke([&id_mock](id_mapper_ref *ref, int, int, crmna_err_t *) {
                  id_mock.set_ref(ref);
                }),
                Return(true)))
      .WillOnce(DoAll(
          Invoke([&toots_mock](id_mapper_ref *ref, int, int, crmna_err_t *) {
            toots_mock.set_ref(ref);
          }),
          Return(true)));
  auto id_mapper_factory = id_factory_mock.get_factory();

  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return(&device));
  auto alloc = alloc_mock.get_ref();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, NULL, &test_logger::logger_ref, &alloc, &err);
  ASSERT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_TRUE(result);
  // EXPECT_STREQ(err_msg, "err_create_locker");
}

TEST(test, test) {
  test_id_mapper_factory_mock id_mock;
  EXPECT_CALL(id_mock, create_id_mapper(_, _, _, _))
      .WillOnce(Return(true))
      .WillOnce(Return(false));
  auto id_mapper_factory = id_mock.get_factory();
  id_mapper_ref ref;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  EXPECT_TRUE(create_id_mapper(&id_mapper_factory, &ref, &err));
  EXPECT_FALSE(create_id_mapper(&id_mapper_factory, &ref, &err));
}
