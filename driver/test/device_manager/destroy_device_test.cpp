#include "../mock/mocks.h"
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
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Pointee;
using ::testing::Return;

TEST(destroy_device, destroy_device_sucess) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock.mocks[0],
              send_message(10, CRMNA_CREATE_DEVICE_RESULT, _, _))
      .WillOnce(Return(10));
  EXPECT_CALL(com_factory_mock.mocks[0],
              send_message(10, CRMNA_DESTROY_DEVICE_RESULT, _, _))
      .WillOnce(Return(10));
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  test_allocater_mock alloc_mock;
  EXPECT_CALL(alloc_mock, free(&alloc_mock.devices[0])).WillOnce(Return());
  test_device_file_factory_mock device_file_mock;

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL,
      device_file_mock.get_mock_factory(), &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  ASSERT_TRUE(result);

  unsigned char create_message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                                    0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t create_mess_buf = {.buf = (char *)create_message,
                                 .buf_size = sizeof(create_message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &create_mess_buf, &err);
  EXPECT_TRUE(result);

  unsigned char destroy_message[] = {0x91, 0x01};
  crmna_buf_t destroy_mess_buf = {.buf = (char *)destroy_message,
                                  .buf_size = sizeof(destroy_message)};

  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_DESTROY_DEVICE, &destroy_mess_buf, &err);
  EXPECT_TRUE(result);
  out_err(&err);
}

TEST(destroy_device, destroy_device_fail_invalid_message) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  test_allocater_mock alloc_mock;
  test_device_file_factory_mock device_file_mock;

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL, device_file_mock.get_mock_factory(),
      &test_logger::logger_ref, alloc_mock.get_mock_ref(), &err);
  ASSERT_TRUE(result);

  unsigned char destroy_message[] = {0x91, 0xcf, 0x00, 0x00, 0x09,
                                     0x29, 0x00, 0x00, 0x09};
  crmna_buf_t destroy_mess_buf = {.buf = (char *)destroy_message,
                                  .buf_size = sizeof(destroy_message)};

  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10, CRMNA_DESTROY_DEVICE,
                         &destroy_mess_buf, &err);
  EXPECT_FALSE(result);
  out_err(&err);
}

TEST(destroy_device, destroy_device_fail_not_fond) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  test_allocater_mock alloc_mock;
  test_device_file_factory_mock device_file_mock;

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL,
      device_file_mock.get_mock_factory(), &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  ASSERT_TRUE(result);

  unsigned char create_message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                                    0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t create_mess_buf = {.buf = (char *)create_message,
                                 .buf_size = sizeof(create_message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &create_mess_buf, &err);
  EXPECT_TRUE(result);

  unsigned char destroy_message[] = {0x91, 0x03};
  crmna_buf_t destroy_mess_buf = {.buf = (char *)destroy_message,
                                  .buf_size = sizeof(destroy_message)};

  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_DESTROY_DEVICE, &destroy_mess_buf, &err);
  EXPECT_FALSE(result);
  out_err(&err);
}

TEST(destroy_device, destroy_device_fail_diffarent_pid) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  test_allocater_mock alloc_mock;
  test_device_file_factory_mock device_file_mock;

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL,
      device_file_mock.get_mock_factory(), &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  ASSERT_TRUE(result);

  unsigned char create_message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                                    0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t create_mess_buf = {.buf = (char *)create_message,
                                 .buf_size = sizeof(create_message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &create_mess_buf, &err);
  EXPECT_TRUE(result);

  unsigned char destroy_message[] = {0x91, 0x01};
  crmna_buf_t destroy_mess_buf = {.buf = (char *)destroy_message,
                                  .buf_size = sizeof(destroy_message)};

  result = reciveMessage(com_factory_mock.mocks[0].communicate, 28,
                         CRMNA_DESTROY_DEVICE, &destroy_mess_buf, &err);
  EXPECT_FALSE(result);
  out_err(&err);
}

TEST(destroy_device, destroy_device_fail_remove) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], remove(1, _, _))
      .WillOnce(Invoke([](int, void **, crmna_err *err) {
        ADD_ERROR(err, "cant remove");
        return false;
      }));
  test_allocater_mock alloc_mock;
  test_device_file_factory_mock device_file_mock;

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL,
      device_file_mock.get_mock_factory(), &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  ASSERT_TRUE(result);

  unsigned char create_message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                                    0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t create_mess_buf = {.buf = (char *)create_message,
                                 .buf_size = sizeof(create_message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &create_mess_buf, &err);
  EXPECT_TRUE(result);

  unsigned char destroy_message[] = {0x91, 0x01};
  crmna_buf_t destroy_mess_buf = {.buf = (char *)destroy_message,
                                  .buf_size = sizeof(destroy_message)};

  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_DESTROY_DEVICE, &destroy_mess_buf, &err);
  EXPECT_FALSE(result);
  out_err(&err);
}

TEST(destroy_device, destroy_device_fail_send_result) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock.mocks[0],
              send_message(10, CRMNA_CREATE_DEVICE_RESULT, _, _))
      .WillOnce(Return(10));
  EXPECT_CALL(com_factory_mock.mocks[0],
              send_message(10, CRMNA_DESTROY_DEVICE_RESULT, _, _))
      .WillOnce(Return(-1));
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  test_allocater_mock alloc_mock;
  EXPECT_CALL(alloc_mock, free(&alloc_mock.devices[0])).WillOnce(Return());
  test_device_file_factory_mock device_file_mock;

  bool result = init_device_manager(
      &device_manager, 0, 0, com_factory_mock.get_mock_factory(),
      locker_factory_mock.get_mock_factory(),
      id_factory_mock.get_mock_factory(), NULL,
      device_file_mock.get_mock_factory(), &test_logger::logger_ref,
      alloc_mock.get_mock_ref(), &err);
  ASSERT_TRUE(result);

  unsigned char create_message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                                    0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t create_mess_buf = {.buf = (char *)create_message,
                                 .buf_size = sizeof(create_message)};
  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_CREATE_DEVICE, &create_mess_buf, &err);
  EXPECT_TRUE(result);

  unsigned char destroy_message[] = {0x91, 0x01};
  crmna_buf_t destroy_mess_buf = {.buf = (char *)destroy_message,
                                  .buf_size = sizeof(destroy_message)};

  result = reciveMessage(com_factory_mock.mocks[0].communicate, 10,
                         CRMNA_DESTROY_DEVICE, &destroy_mess_buf, &err);
  EXPECT_FALSE(result);
  out_err(&err);
}