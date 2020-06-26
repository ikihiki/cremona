#include "../mock/test_allocater.h"
#include "../mock/test_communicator.h"
#include "../mock/test_device_file.h"
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
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_factory_mock id_mock;
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
  auto com_factory = com_factory_mock.get_factory();
  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_mock;
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
  auto com_factory = com_factory_mock.get_factory();
  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *, crmna_err_t *err) {
                        snprintf(err->error_msg, err->error_msg_len,
                                 "error_add_device");
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
  EXPECT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  EXPECT_STRNE(err_msg, "");
}

TEST(device_manager, create_device_message_fail_create_locker) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
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

  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *id, crmna_err_t *) { *id = 1; }),
                      Return(true)));
  EXPECT_CALL(id_factory_mock.mocks[0], remove(1, _)).WillOnce(Return(true));
  auto id_mapper_factory = id_factory_mock.get_factory();

  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return(&device));
  EXPECT_CALL(alloc_mock, free(&device)).WillOnce(Return());
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

TEST(device_manager, create_device_message_fail_create_id_mapper) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
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

TEST(device_manager, create_device_message_fail_create_device_file) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
  EXPECT_CALL(locker_factory_mock.mocks[1], free(_)).WillOnce(Return(true));
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *id, crmna_err_t *) { *id = 1; }),
                      Return(true)));
  EXPECT_CALL(id_factory_mock.mocks[0], remove(1, _)).WillOnce(Return(true));
  EXPECT_CALL(id_factory_mock.mocks[1], free(_)).WillOnce(Return(true));
  auto id_mapper_factory = id_factory_mock.get_factory();

  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return(&device));
  EXPECT_CALL(alloc_mock, free(_));
  auto alloc = alloc_mock.get_ref();

  test_device_file_mock device_file_mock;
  test_device_file_factory_mock device_file_factory_mock;
  EXPECT_CALL(device_file_factory_mock, create_device_file(_, _, _))
      .WillOnce(Return(false));
  auto device_file_factory = device_file_factory_mock.get_factory();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, &device_file_factory, &test_logger::logger_ref, &alloc, &err);
  ASSERT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  // EXPECT_STREQ(err_msg, "err_create_locker");
}

TEST(device_manager, create_device_message_fail_send_compleate_message) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock.mocks[0],
              send_message(10, CRMNA_CREATE_DEVICE_RESULT, _, _))
      .WillOnce(Return(-1));
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *id, crmna_err_t *) { *id = 1; }),
                      Return(true)));
  EXPECT_CALL(id_factory_mock.mocks[0], remove(1, _)).WillOnce(Return(true));
  auto id_mapper_factory = id_factory_mock.get_factory();

  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return(&device));
  EXPECT_CALL(alloc_mock, free(_));
  auto alloc = alloc_mock.get_ref();

  test_device_file_factory_mock device_file_mock;
  EXPECT_CALL(device_file_mock.mocks[0], free(_)).WillOnce(Return(true));
  auto device_factory = device_file_mock.get_factory();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, &device_factory, &test_logger::logger_ref, &alloc, &err);
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
  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock.mocks[0],
              send_message(10, CRMNA_CREATE_DEVICE_RESULT, _, _))
      .WillOnce(Return(10));
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();

  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return(&device));
  auto alloc = alloc_mock.get_ref();

  test_device_file_factory_mock device_file_mock;
  auto device_factory = device_file_mock.get_factory();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, &device_factory, &test_logger::logger_ref, &alloc, &err);
  ASSERT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_TRUE(result);
  // EXPECT_STREQ(err_msg, "err_create_locker");
  std::cout << err_msg << std::endl;
}

