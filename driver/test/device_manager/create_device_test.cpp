#include "../mock/test_allocater.h"
#include "../mock/test_communicator.h"
#include "../mock/test_device_file.h"
#include "../mock/test_id_mapper.h"
#include "../mock/test_locker.h"
#include "../mock/test_logger.h"
#include "../test_util.h"
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
using ::testing::DoDefault;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Pointee;
using ::testing::Return;

TEST(device_manager, create_device_message_invalid_message) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;

  bool result = init_device_manager(&device_manager, 0, 0,
                                    com_factory_mock.get_mock_factory(),
                                    locker_factory_mock.get_mock_factory(),
                                    id_factory_mock.get_mock_factory(), NULL,
                                    NULL, &test_logger::logger_ref, NULL, &err);
  EXPECT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  EXPECT_TRUE(HAS_ERROR(&err));
  out_err(&err);
}

TEST(device_manager, create_device_message_fail_allocation) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  test_allocater_mock alloc_mock;
  EXPECT_CALL(alloc_mock, allocate(_)).WillOnce(Return((void *)NULL));

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL, NULL, &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  EXPECT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  EXPECT_TRUE(HAS_ERROR(&err));
  out_err(&err);
}

TEST(device_manager, create_device_message_fail_add_device) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *, crmna_err *err) {
                        ADD_ERROR(err, "error_add_device");
                      }),
                      Return(false)));
  test_allocater_mock alloc_mock;

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL, NULL, &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  EXPECT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  EXPECT_TRUE(HAS_ERROR(&err));
  out_err(&err);
}

TEST(device_manager, create_device_message_fail_create_locker) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  test_locker_mock locker_mock;
  test_locker_factory_mock locker_factory_mock;
  EXPECT_CALL(locker_factory_mock, create_locker(_, _))
      .WillOnce(Return(true))
      .WillOnce(Return(true))
      .WillOnce(DoAll(Invoke([](locker_ref *, crmna_err *err) {
                        ADD_ERROR(err, "err_create_locker");
                      }),
                      Return(false)));
  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *id, crmna_err *) { *id = 1; }),
                      Return(true)));
  EXPECT_CALL(id_factory_mock.mocks[0], remove(1, _, _)).WillOnce(Return(true));
  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return(&device));
  EXPECT_CALL(alloc_mock, free(&device)).WillOnce(Return());

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL, NULL, &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  ASSERT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 0,
                         CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  EXPECT_TRUE(HAS_ERROR(&err));
  out_err(&err);
}

TEST(device_manager, create_device_message_fail_create_id_mapper) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_mock id_mock;
  EXPECT_CALL(id_mock, add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *id, crmna_err *) { *id = 1; }),
                      Return(true)));
  EXPECT_CALL(id_mock, remove(1, _, _)).WillOnce(Return(true));

  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock, create_id_mapper(_, _, _, _))
      .WillOnce(DoAll(Invoke([&id_mock](id_mapper_ref *ref, int, int,
                                        crmna_err *) { id_mock.set_ref(ref); }),
                      Return(true)))
      .WillOnce(DoDefault())
      .WillOnce(DoDefault())
      .WillOnce(DoDefault())
      .WillOnce(DoAll(Invoke([](id_mapper_ref *, int, int, crmna_err *err) {
                        ADD_ERROR(err, "error_id_mapper");
                      }),
                      Return(false)));
  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return((void *)&device));
  EXPECT_CALL(alloc_mock, free(Eq(&device))).WillOnce(Return());

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL, NULL, &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  ASSERT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  EXPECT_TRUE(HAS_ERROR(&err));
  out_err(&err);
}

TEST(device_manager, create_device_message_fail_create_device_file) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  test_locker_factory_mock locker_factory_mock;
  EXPECT_CALL(locker_factory_mock.mocks[2], free()).WillOnce(Return());
  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *id, crmna_err *) { *id = 1; }),
                      Return(true)));
  EXPECT_CALL(id_factory_mock.mocks[0], remove(1, _, _)).WillOnce(Return(true));
  EXPECT_CALL(id_factory_mock.mocks[4], free()).WillOnce(Return());
  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return(&device));
  EXPECT_CALL(alloc_mock, free(_));
  test_device_file_factory_mock device_file_factory_mock;
  EXPECT_CALL(device_file_factory_mock, create_device_file(_, _, _))
      .WillOnce(Return(false));

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL,
      device_file_factory_mock.get_mock_factory(), &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  ASSERT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  EXPECT_TRUE(HAS_ERROR(&err));
  out_err(&err);
}

TEST(device_manager, create_device_message_fail_send_compleate_message) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock.mocks[0],
              send_message(10, CRMNA_CREATE_DEVICE_RESULT, _, _))
      .WillOnce(Return(-1));
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return(&device));
  test_device_file_factory_mock device_file_mock;

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL,
      device_file_mock.get_mock_factory(), &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  ASSERT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  EXPECT_TRUE(HAS_ERROR(&err));
  out_err(&err);
}

TEST(device_manager, create_device_message_fail_replace_device) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], add_get_id(_, _, _))
      .WillOnce(DoAll(Invoke([](void *, int *id, crmna_err *) { *id = 1; }),
                      Return(true)));
  EXPECT_CALL(id_factory_mock.mocks[0], remove(1, _, _)).WillOnce(Return(true));
  EXPECT_CALL(id_factory_mock.mocks[0], replace(1, _, _, _))
      .WillOnce(Return(false));
  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return(&device));
  EXPECT_CALL(alloc_mock, free(_));
  test_device_file_factory_mock device_file_mock;
  EXPECT_CALL(device_file_mock.mocks[0], free()).WillOnce(Return());

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL,
      device_file_mock.get_mock_factory(), &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  ASSERT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_FALSE(result);
  EXPECT_TRUE(HAS_ERROR(&err));
  out_err(&err);
}

TEST(device_manager, create_device_message_success) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock.mocks[0],
              send_message(10, CRMNA_CREATE_DEVICE_RESULT, _, _))
      .WillOnce(Return(10));
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  test_allocater_mock alloc_mock;
  cremona_device_t device;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return(&device));
  test_device_file_factory_mock device_file_mock;

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL,
      device_file_mock.get_mock_factory(), &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  ASSERT_TRUE(result);

  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &buf, &err);
  EXPECT_TRUE(result);
  EXPECT_TRUE(id_factory_mock.mocks[0].map.find(1) !=
              id_factory_mock.mocks[0].map.end());
  out_err(&err);
}
