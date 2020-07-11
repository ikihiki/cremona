#include "../mock/mocks.h"
#include "../test_util.h"
#include "cremona.h"
#include "cremona_internal.h"
#include "src/message.h"
#include <chrono>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Pointee;
using ::testing::Return;
using ::testing::ReturnNull;

TEST(device, open_toot_fail_already_destroied) {
  DEFINE_ERROR(err);
  test_communicate communicate_mock(&test_logger::logger_ref);
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  test_allocater_mock alloc_mock;
  test_device_file_factory_mock device_file_mock;
  test_waiter_factory_mock waiter_factory_mock;

  auto device =
      create_device(1, 10, 28, "dev_name", id_factory_mock.get_mock_factory(),
                    locker_factory_mock.get_mock_factory(),
                    communicate_mock.get_communicate(),
                    waiter_factory_mock.get_mock_factory(),
                    device_file_mock.get_mock_factory(),
                    &test_logger::logger_ref, alloc_mock.get_mock_ref(), &err);

  destroy_device(device);
  auto toot = open_toot(device, false, &err);
  EXPECT_EQ(NULL, toot);
  out_err(&err);
}

TEST(device, open_toot_fail_assign_id) {
  DEFINE_ERROR(err);

  test_communicate communicate_mock(&test_logger::logger_ref);
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], add_get_id(_, _, _))
      .WillOnce(Invoke([](void *, int *, crmna_err *err) {
        ADD_ERROR(err, "error_add_get_id");
        return false;
      }));
  test_allocater_mock alloc_mock;
  test_device_file_factory_mock device_file_mock;
  test_waiter_factory_mock waiter_factory_mock;

  auto device =
      create_device(1, 10, 28, "dev_name", id_factory_mock.get_mock_factory(),
                    locker_factory_mock.get_mock_factory(),
                    communicate_mock.get_communicate(),
                    waiter_factory_mock.get_mock_factory(),
                    device_file_mock.get_mock_factory(),
                    &test_logger::logger_ref, alloc_mock.get_mock_ref(), &err);

  auto toot = open_toot(device, false, &err);
  EXPECT_EQ(NULL, toot);
  out_err(&err);
}

TEST(device, open_toot_fail_cretate_toot) {
  DEFINE_ERROR(err);

  test_communicate communicate_mock(&test_logger::logger_ref);
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  test_allocater_mock alloc_mock;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return((void *)&(alloc_mock.devices[0])));
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_toot_t)))
      .WillOnce(ReturnNull());
  test_device_file_factory_mock device_file_mock;
  test_waiter_factory_mock waiter_factory_mock;

  auto device =
      create_device(1, 10, 28, "dev_name", id_factory_mock.get_mock_factory(),
                    locker_factory_mock.get_mock_factory(),
                    communicate_mock.get_communicate(),
                    waiter_factory_mock.get_mock_factory(),
                    device_file_mock.get_mock_factory(),
                    &test_logger::logger_ref, alloc_mock.get_mock_ref(), &err);

  auto toot = open_toot(device, false, &err);
  EXPECT_EQ(NULL, toot);
  out_err(&err);
}

TEST(device, open_toot_fail_replace) {
  DEFINE_ERROR(err);

  test_communicate communicate_mock(&test_logger::logger_ref);
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], replace(_, _, _, _))
      .WillOnce(Return(false));
  test_allocater_mock alloc_mock;
  test_device_file_factory_mock device_file_mock;
  test_waiter_factory_mock waiter_factory_mock;

  auto device =
      create_device(1, 10, 28, "dev_name", id_factory_mock.get_mock_factory(),
                    locker_factory_mock.get_mock_factory(),
                    communicate_mock.get_communicate(),
                    waiter_factory_mock.get_mock_factory(),
                    device_file_mock.get_mock_factory(),
                    &test_logger::logger_ref, alloc_mock.get_mock_ref(), &err);

  auto toot = open_toot(device, false, &err);
  EXPECT_EQ(NULL, toot);
  out_err(&err);
}

TEST(device, open_toot_success) {
  DEFINE_ERROR(err);

  test_communicate communicate_mock(&test_logger::logger_ref);
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  test_allocater_mock alloc_mock;
  test_device_file_factory_mock device_file_mock;
  test_waiter_factory_mock waiter_factory_mock;
  auto waiter_factory = waiter_factory_mock.get_factory();

  auto device =
      create_device(1, 10, 28, "dev_name", id_factory_mock.get_mock_factory(),
                    locker_factory_mock.get_mock_factory(),
                    communicate_mock.get_communicate(),
                    waiter_factory_mock.get_mock_factory(),
                    device_file_mock.get_mock_factory(),
                    &test_logger::logger_ref, alloc_mock.get_mock_ref(), &err);

  auto toot = open_toot(device, false, &err);
  EXPECT_NE(nullptr, toot);
  out_err(&err);
}
