#include "../mock/test_allocater.h"
#include "../mock/test_communicator.h"
#include "../mock/test_device_file.h"
#include "../mock/test_id_mapper.h"
#include "../mock/test_locker.h"
#include "../mock/test_logger.h"
#include "../mock/test_waiter.h"
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
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};

  communicator_ref com;
  test_communicator_mock com_mock;
  com_mock.set_ref(&com);
  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_file_factory = device_file_mock.get_factory();
  test_waiter_factory_mock waiter_factory_mock;
  auto waiter_factory = waiter_factory_mock.get_factory();

  auto device =
      create_device(1, 10, 28, "dev_name", &id_mapper_factory, &locker_factory,
                    &com, &waiter_factory, &device_file_factory,
                    &test_logger::logger_ref, &alloc, &err);
  destroy_device(device);
  auto toot = open_toot(device, true, &err);
  EXPECT_EQ(NULL, toot);
  std::cout << err_msg << std::endl;
}

TEST(device, open_toot_fail_assign_id) {
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};

  communicator_ref com;
  test_communicator_mock com_mock;
  com_mock.set_ref(&com);
  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], add_get_id(_, _, _))
      .WillOnce(Invoke([](void *, int *, crmna_err_t *err) {
        snprintf(err->error_msg, err->error_msg_len, "error_add_get_id");
        return false;
      }));
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_file_factory = device_file_mock.get_factory();
  test_waiter_factory_mock waiter_factory_mock;
  auto waiter_factory = waiter_factory_mock.get_factory();

  auto device =
      create_device(1, 10, 28, "dev_name", &id_mapper_factory, &locker_factory,
                    &com, &waiter_factory, &device_file_factory,
                    &test_logger::logger_ref, &alloc, &err);

  auto toot = open_toot(device, true, &err);
  EXPECT_EQ(NULL, toot);
  std::cout << err_msg << std::endl;
}

TEST(device, open_toot_fail_cretate_toot) {
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};

  communicator_ref com;
  test_communicator_mock com_mock;
  com_mock.set_ref(&com);
  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_device_t)))
      .WillOnce(Return((void *)&(alloc_mock.devices[0])));
  EXPECT_CALL(alloc_mock, allocate(sizeof(cremona_toot_t)))
      .WillOnce(ReturnNull());
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_file_factory = device_file_mock.get_factory();
  test_waiter_factory_mock waiter_factory_mock;
  auto waiter_factory = waiter_factory_mock.get_factory();
  auto device =
      create_device(1, 10, 28, "dev_name", &id_mapper_factory, &locker_factory,
                    &com, &waiter_factory, &device_file_factory,
                    &test_logger::logger_ref, &alloc, &err);
  auto toot = open_toot(device, true, &err);
  EXPECT_EQ(NULL, toot);
  std::cout << err_msg << std::endl;
}

TEST(device, open_toot_fail_replace) {
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};

  communicator_ref com;
  test_communicator_mock com_mock;
  com_mock.set_ref(&com);
  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  EXPECT_CALL(id_factory_mock.mocks[0], replace(_, _, _, _))
      .WillOnce(Return(false));
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_file_factory = device_file_mock.get_factory();
  test_waiter_factory_mock waiter_factory_mock;
  auto waiter_factory = waiter_factory_mock.get_factory();
  auto device =
      create_device(1, 10, 28, "dev_name", &id_mapper_factory, &locker_factory,
                    &com, &waiter_factory, &device_file_factory,
                    &test_logger::logger_ref, &alloc, &err);
  auto toot = open_toot(device, true, &err);
  EXPECT_EQ(NULL, toot);
  std::cout << err_msg << std::endl;
}

TEST(device, open_toot_fail_send_new_toot) {
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};

  communicator_ref com;
  test_communicator_mock com_mock;
  EXPECT_CALL(com_mock, send_message(_, _, _, _)).WillOnce(Return(-1));
  com_mock.set_ref(&com);
  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_file_factory = device_file_mock.get_factory();
  test_waiter_factory_mock waiter_factory_mock;
  auto waiter_factory = waiter_factory_mock.get_factory();
  auto device =
      create_device(1, 10, 28, "dev_name", &id_mapper_factory, &locker_factory,
                    &com, &waiter_factory, &device_file_factory,
                    &test_logger::logger_ref, &alloc, &err);
  auto toot = open_toot(device, true, &err);
  EXPECT_EQ(NULL, toot);
  std::cout << err_msg << std::endl;
}

TEST(device, open_toot_fail_state) {
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};

  communicator_ref com;
  test_communicator_mock com_mock;
  com_mock.set_ref(&com);
  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_file_factory = device_file_mock.get_factory();
  test_waiter_factory_mock waiter_factory_mock;
  auto waiter_factory = waiter_factory_mock.get_factory();
  auto device =
      create_device(1, 10, 28, "dev_name", &id_mapper_factory, &locker_factory,
                    &com, &waiter_factory, &device_file_factory,
                    &test_logger::logger_ref, &alloc, &err);

  std::thread t([&waiter_factory_mock]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    auto ref = waiter_factory_mock.mocks[0].get_ref();
    waiter_notify(&ref, NULL);
  });

  auto toot = open_toot(device, true, &err);
  t.join();
  EXPECT_EQ(NULL, toot);
  std::cout << err_msg << std::endl;
}

TEST(device, open_toot_success_wait) {
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};

  communicator_ref com;
  test_communicator_mock com_mock;
  com_mock.set_ref(&com);
  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_file_factory = device_file_mock.get_factory();
  test_waiter_factory_mock waiter_factory_mock;
  auto waiter_factory = waiter_factory_mock.get_factory();
  auto device =
      create_device(1, 10, 28, "dev_name", &id_mapper_factory, &locker_factory,
                    &com, &waiter_factory, &device_file_factory,
                    &test_logger::logger_ref, &alloc, &err);

  std::thread t([&waiter_factory_mock, &alloc_mock]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto ref = &(waiter_factory_mock.mocks[0]);
    {
      std::lock_guard<std::mutex> lk(ref->mtx);
      alloc_mock.toots[0].state = OPEND;
    }
    ref->cond.notify_one();
  });

  auto toot = open_toot(device, true, &err);
  t.join();
  EXPECT_NE(nullptr, toot);
  std::cout << err_msg << std::endl;
}

TEST(device, open_toot_success_no_wait) {
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};

  communicator_ref com;
  test_communicator_mock com_mock;
  com_mock.set_ref(&com);
  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();
  test_id_mapper_factory_mock id_factory_mock;
  auto id_mapper_factory = id_factory_mock.get_factory();
  test_allocater_mock alloc_mock;
  auto alloc = alloc_mock.get_ref();
  test_device_file_factory_mock device_file_mock;
  auto device_file_factory = device_file_mock.get_factory();
  test_waiter_factory_mock waiter_factory_mock;
  auto waiter_factory = waiter_factory_mock.get_factory();
  auto device =
      create_device(1, 10, 28, "dev_name", &id_mapper_factory, &locker_factory,
                    &com, &waiter_factory, &device_file_factory,
                    &test_logger::logger_ref, &alloc, &err);
  auto toot = open_toot(device, false, &err);
  EXPECT_NE(nullptr, toot);
  std::cout << err_msg << std::endl;
}