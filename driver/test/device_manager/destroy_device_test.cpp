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

TEST(destroy_device, destroy_device_sucess) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock.mocks[0],
              send_message(10, CRMNA_CREATE_DEVICE_RESULT, _, _))
      .WillOnce(Return(10));
  EXPECT_CALL(com_factory_mock.mocks[0],
              send_message(10, CRMNA_DESTROY_DEVICE_RESULT, _, _))
      .WillOnce(Return(10));
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  EXPECT_CALL(alloc_mock, free(&alloc_mock.devices[0])).WillOnce(Return());
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_factory = device_file_mock.get_factory();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, &device_factory, &test_logger::logger_ref, &alloc, &err);
  ASSERT_TRUE(result);

  unsigned char create_message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                                    0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t create_mess_buf = {.buf = (char *)create_message,
                                 .buf_size = sizeof(create_message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE,
                         &create_mess_buf, &err);
  EXPECT_TRUE(result);

  unsigned char destroy_message[] = {0x91, 0x01};
  crmna_buf_t destroy_mess_buf = {.buf = (char *)destroy_message,
                                  .buf_size = sizeof(destroy_message)};

  result = reciveMessage(&device_manager, 10, CRMNA_DESTROY_DEVICE,
                         &destroy_mess_buf, &err);
  EXPECT_TRUE(result);
  std::cout << err_msg << std::endl;
}

TEST(destroy_device, destroy_device_fail_invalid_message) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_factory = device_file_mock.get_factory();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, &device_factory, &test_logger::logger_ref, &alloc, &err);
  ASSERT_TRUE(result);

  unsigned char destroy_message[] = {0x91, 0xcf, 0x00, 0x00, 0x09,
                                     0x29, 0x00, 0x00, 0x09};
  crmna_buf_t destroy_mess_buf = {.buf = (char *)destroy_message,
                                  .buf_size = sizeof(destroy_message)};

  result = reciveMessage(&device_manager, 10, CRMNA_DESTROY_DEVICE,
                         &destroy_mess_buf, &err);
  EXPECT_FALSE(result);
  std::cout << err_msg << std::endl;
}

TEST(destroy_device, destroy_device_fail_not_fond) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_factory = device_file_mock.get_factory();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, &device_factory, &test_logger::logger_ref, &alloc, &err);
  ASSERT_TRUE(result);

  unsigned char create_message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                                    0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t create_mess_buf = {.buf = (char *)create_message,
                                 .buf_size = sizeof(create_message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE,
                         &create_mess_buf, &err);
  EXPECT_TRUE(result);

  unsigned char destroy_message[] = {0x91, 0x03};
  crmna_buf_t destroy_mess_buf = {.buf = (char *)destroy_message,
                                  .buf_size = sizeof(destroy_message)};

  result = reciveMessage(&device_manager, 10, CRMNA_DESTROY_DEVICE,
                         &destroy_mess_buf, &err);
  EXPECT_FALSE(result);
  std::cout << err_msg << std::endl;
}

TEST(destroy_device, destroy_device_fail_diffarent_pid) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_factory = device_file_mock.get_factory();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, &device_factory, &test_logger::logger_ref, &alloc, &err);
  ASSERT_TRUE(result);

  unsigned char create_message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                                    0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t create_mess_buf = {.buf = (char *)create_message,
                                 .buf_size = sizeof(create_message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE,
                         &create_mess_buf, &err);
  EXPECT_TRUE(result);

  unsigned char destroy_message[] = {0x91, 0x01};
  crmna_buf_t destroy_mess_buf = {.buf = (char *)destroy_message,
                                  .buf_size = sizeof(destroy_message)};

  result = reciveMessage(&device_manager, 28, CRMNA_DESTROY_DEVICE,
                         &destroy_mess_buf, &err);
  EXPECT_FALSE(result);
  std::cout << err_msg << std::endl;
}

TEST(destroy_device, destroy_device_fail_remove) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();
  EXPECT_CALL(id_factory_mock.mocks[0], remove(1, _, _))
      .WillOnce(Invoke([](int, void **, crmna_err_t *err) {
        snprintf(err->error_msg, err->error_msg_len, "cant remove");
        return false;
      }));
  test_allocater_mock alloc_mock;
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_factory = device_file_mock.get_factory();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, &device_factory, &test_logger::logger_ref, &alloc, &err);
  ASSERT_TRUE(result);

  unsigned char create_message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                                    0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t create_mess_buf = {.buf = (char *)create_message,
                                 .buf_size = sizeof(create_message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE,
                         &create_mess_buf, &err);
  EXPECT_TRUE(result);

  unsigned char destroy_message[] = {0x91, 0x01};
  crmna_buf_t destroy_mess_buf = {.buf = (char *)destroy_message,
                                  .buf_size = sizeof(destroy_message)};

  result = reciveMessage(&device_manager, 10, CRMNA_DESTROY_DEVICE,
                         &destroy_mess_buf, &err);
  EXPECT_FALSE(result);
  std::cout << err_msg << std::endl;
}

TEST(destroy_device, destroy_device_fail_send_result) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock.mocks[0],
              send_message(10, CRMNA_CREATE_DEVICE_RESULT, _, _))
      .WillOnce(Return(10));
  EXPECT_CALL(com_factory_mock.mocks[0],
              send_message(10, CRMNA_DESTROY_DEVICE_RESULT, _, _))
      .WillOnce(Return(-1));
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  EXPECT_CALL(alloc_mock, free(&alloc_mock.devices[0])).WillOnce(Return());
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_factory = device_file_mock.get_factory();

  bool result = init_device_manager(
      &device_manager, 0, 0, &com_factory, &locker_factory, &id_mapper_factory,
      NULL, &device_factory, &test_logger::logger_ref, &alloc, &err);
  ASSERT_TRUE(result);

  unsigned char create_message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                                    0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0a};
  crmna_buf_t create_mess_buf = {.buf = (char *)create_message,
                                 .buf_size = sizeof(create_message)};
  result = reciveMessage(&device_manager, 10, CRMNA_CREATE_DEVICE,
                         &create_mess_buf, &err);
  EXPECT_TRUE(result);

  unsigned char destroy_message[] = {0x91, 0x01};
  crmna_buf_t destroy_mess_buf = {.buf = (char *)destroy_message,
                                  .buf_size = sizeof(destroy_message)};

  result = reciveMessage(&device_manager, 10, CRMNA_DESTROY_DEVICE,
                         &destroy_mess_buf, &err);
  EXPECT_FALSE(result);
  std::cout << err_msg << std::endl;
}