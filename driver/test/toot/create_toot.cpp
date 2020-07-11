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

TEST(toot, create_toot_fail_allocation) {
  test_allocater_mock alloc_mock;
  EXPECT_CALL(alloc_mock, allocate(_)).WillOnce(Return((void *)NULL));
  DEFINE_ERROR(err);

  auto toot = create_toot(1, 1, 1, alloc_mock.get_mock_ref(), NULL, NULL, NULL,
                          false, &test_logger::logger_ref, &err);

  EXPECT_EQ(toot, nullptr);
  out_err(&err);
}

TEST(toot, create_toot_fail_create_locker) {
  test_allocater_mock alloc_mock;
  test_locker_factory_mock locker_factory_mock;
  EXPECT_CALL(locker_factory_mock, create_locker(_, _))
      .WillOnce(DoAll(Invoke([](locker_ref *, crmna_err *err) {
                        ADD_ERROR(err, "err_create_locker");
                      }),
                      Return(false)));
  DEFINE_ERROR(err);

  auto toot = create_toot(1, 1, 1, alloc_mock.get_mock_ref(),
                          locker_factory_mock.get_mock_factory(), NULL, NULL,
                          false, &test_logger::logger_ref, &err);

  EXPECT_EQ(toot, nullptr);
  EXPECT_TRUE(HAS_ERROR(&err));
  out_err(&err);
}

TEST(toot, create_toot_fail_create_waiter) {
  test_allocater_mock alloc_mock;
  test_locker_factory_mock locker_factory_mock;
  test_waiter_factory_mock waiter_factory_mock;
  EXPECT_CALL(waiter_factory_mock, create_waiter(_, _))
      .WillOnce(DoAll(Invoke([](waiter_ref *, crmna_err *err) {
                        ADD_ERROR(err, "err_create_waiter");
                      }),
                      Return(false)));
  DEFINE_ERROR(err);

  auto toot = create_toot(1, 1, 1, alloc_mock.get_mock_ref(),
                          locker_factory_mock.get_mock_factory(),
                          waiter_factory_mock.get_mock_factory(), NULL, false,
                          &test_logger::logger_ref, &err);

  EXPECT_EQ(toot, nullptr);
  EXPECT_TRUE(HAS_ERROR(&err));
  out_err(&err);
}

TEST(toot, create_toot_communicater_null) {
  test_allocater_mock alloc_mock;
  test_locker_factory_mock locker_factory_mock;
  test_waiter_factory_mock waiter_factory_mock;
  DEFINE_ERROR(err);

  auto toot = create_toot(1, 1, 1, alloc_mock.get_mock_ref(),
                          locker_factory_mock.get_mock_factory(),
                          waiter_factory_mock.get_mock_factory(), NULL, false,
                          &test_logger::logger_ref, &err);

  EXPECT_EQ(toot, nullptr);
  EXPECT_TRUE(HAS_ERROR(&err));
  out_err(&err);
}

TEST(toot, create_toot_fail_send_new_toot_message) {
  test_allocater_mock alloc_mock;
  test_locker_factory_mock locker_factory_mock;
  test_waiter_factory_mock waiter_factory_mock;
  test_communicate communicate_mock(&test_logger::logger_ref);
  EXPECT_CALL(communicate_mock.communicator_factory.mocks[0],
              send_message(1, CRMNA_CREATE_DEVICE_RESULT, _, _))
      .WillOnce(Return(-1));
  DEFINE_ERROR(err);

  auto toot = create_toot(1, 1, 1, alloc_mock.get_mock_ref(),
                          locker_factory_mock.get_mock_factory(),
                          waiter_factory_mock.get_mock_factory(),
                          communicate_mock.get_communicate(), false,
                          &test_logger::logger_ref, &err);

  EXPECT_EQ(toot, nullptr);
  EXPECT_TRUE(HAS_ERROR(&err));
  out_err(&err);
}

TEST(toot, create_toot_fail_state) {
  test_allocater_mock alloc_mock;
  test_locker_factory_mock locker_factory_mock;
  test_waiter_factory_mock waiter_factory_mock;
  test_communicate communicate_mock(&test_logger::logger_ref);
  DEFINE_ERROR(err);

  auto toot = create_toot(1, 1, 1, alloc_mock.get_mock_ref(),
                          locker_factory_mock.get_mock_factory(),
                          waiter_factory_mock.get_mock_factory(),
                          communicate_mock.get_communicate(), true,
                          &test_logger::logger_ref, &err);

  std::thread t([&waiter_factory_mock]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    auto ref = waiter_factory_mock.mocks[0].get_ref();
    waiter_notify(&ref);
  });

  EXPECT_EQ(toot, nullptr);
  EXPECT_TRUE(HAS_ERROR(&err));
  t.join();
  out_err(&err);
}

TEST(toot, create_toot_cancel_timeout) {
  test_allocater_mock alloc_mock;
  test_locker_factory_mock locker_factory_mock;
  test_waiter_factory_mock waiter_factory_mock;
  test_communicate communicate_mock(&test_logger::logger_ref);
  DEFINE_ERROR(err);

  auto toot = create_toot(1, 1, 1, alloc_mock.get_mock_ref(),
                          locker_factory_mock.get_mock_factory(),
                          waiter_factory_mock.get_mock_factory(),
                          communicate_mock.get_communicate(), true,
                          &test_logger::logger_ref, &err);

  std::thread t([&waiter_factory_mock, &communicate_mock]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    unsigned char message[] = {0x94, 0x01, 0x01, 0x01, 0xc3};
    crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
    reciveMessage(communicate_mock.communicator_factory.mocks[0].communicate, 1,
                  CRMNA_NEW_TOOT_RESULT, &buf, NULL);
  });

  t.join();
  EXPECT_EQ(toot, nullptr);
  EXPECT_TRUE(HAS_ERROR(&err));

  out_err(&err);
}

TEST(toot, create_toot_success_wait) {
  test_allocater_mock alloc_mock;
  test_locker_factory_mock locker_factory_mock;
  test_waiter_factory_mock waiter_factory_mock;
  test_communicate communicate_mock(&test_logger::logger_ref);
  DEFINE_ERROR(err);

  std::thread t([&waiter_factory_mock, &communicate_mock]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    unsigned char message[] = {0x94, 0x01, 0x01, 0x01, 0xc3};
    crmna_buf_t buf = {.buf = (char *)message, .buf_size = sizeof(message)};
    reciveMessage(communicate_mock.communicator_factory.mocks[0].communicate, 1,
                  CRMNA_NEW_TOOT_RESULT, &buf, NULL);
  });

  auto toot = create_toot(1, 1, 1, alloc_mock.get_mock_ref(),
                          locker_factory_mock.get_mock_factory(),
                          waiter_factory_mock.get_mock_factory(),
                          communicate_mock.get_communicate(), true,
                          &test_logger::logger_ref, &err);

  t.join();
  EXPECT_NE(toot, nullptr);
  EXPECT_FALSE(HAS_ERROR(&err));

  out_err(&err);
}

TEST(toot, create_toot_success) {
  test_allocater_mock alloc_mock;
  test_locker_factory_mock locker_factory_mock;
  test_waiter_factory_mock waiter_factory_mock;
  test_communicate communicate_mock(&test_logger::logger_ref);
  DEFINE_ERROR(err);

  auto toot = add_ref_toot(create_toot(1, 1, 1, alloc_mock.get_mock_ref(),
                                       locker_factory_mock.get_mock_factory(),
                                       waiter_factory_mock.get_mock_factory(),
                                       communicate_mock.get_communicate(),
                                       false, &test_logger::logger_ref, &err));

  EXPECT_NE(toot, nullptr);
  EXPECT_FALSE(HAS_ERROR(&err));
  out_err(&err);
}