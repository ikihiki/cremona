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

using ::testing::_;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Pointee;
using ::testing::Return;

TEST(device_manager, init_device_manager_fail_create_communicator) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_mock;
  EXPECT_CALL(com_mock, create_communicator(_, _, _))
      .WillOnce(DoAll(Invoke([](cremona_device_manager_t *, communicator_ref *,
                                crmna_err_t *err) {
                        snprintf(err->error_msg, err->error_msg_len,
                                 "error_communicator");
                      }),
                      Return(false)));
  auto com_factory = com_mock.get_factory();
  bool result = init_device_manager(&device_manager, 0, 0, &com_factory, NULL,
                                    NULL, NULL, NULL, NULL, NULL, &err);

  EXPECT_FALSE(result);
  EXPECT_STREQ(err_msg, "error_communicator");
} // namespace )

TEST(device_manager, init_device_manager_fail_create_locker) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_mock;
  EXPECT_CALL(locker_mock, create_locker(_, _))
      .WillOnce(DoAll(Invoke([](locker_ref *, crmna_err_t *err) {
                        snprintf(err->error_msg, err->error_msg_len,
                                 "error_locker");
                      }),
                      Return(false)));
  auto locker_factory = locker_mock.get_factory();

  bool result =
      init_device_manager(&device_manager, 0, 0, &com_factory, &locker_factory,
                          NULL, NULL, NULL, NULL, NULL, &err);

  EXPECT_FALSE(result);
  EXPECT_STREQ(err_msg, "error_locker");
}

TEST(device_manager, init_device_manager_fail_create_id_mapper) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};
  test_communicator_factory_mock com_factory_mock;
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_factory_mock id_mock;
  EXPECT_CALL(id_mock, create_id_mapper(_, _, _, _))
      .WillOnce(Invoke([](id_mapper_ref *, int, int, crmna_err_t *err) {
        snprintf(err->error_msg, err->error_msg_len, "error_id_mapper");
        return false;
      }));
  auto id_mapper_factory = id_mock.get_factory();

  bool result =
      init_device_manager(&device_manager, 0, 0, &com_factory, &locker_factory,
                          &id_mapper_factory, NULL, NULL, NULL, NULL, &err);

  EXPECT_FALSE(result);
  EXPECT_STREQ(err_msg, "error_id_mapper");
}

TEST(device_manager, init_device_manager_success) {
  cremona_device_manager_t device_manager;
  char err_msg[100];
  crmna_err_t err = {.error_msg = err_msg, .error_msg_len = sizeof(err_msg)};

  test_communicator_factory_mock com_factory_mock;
  auto com_factory = com_factory_mock.get_factory();
  test_locker_factory_mock locker_mock;
  auto locker_factory = locker_mock.get_factory();
  test_id_mapper_factory_mock id_mock;
  auto id_mapper_factory = id_mock.get_factory();

  bool result =
      init_device_manager(&device_manager, 0, 0, &com_factory, &locker_factory,
                          &id_mapper_factory, NULL, NULL, NULL, NULL, &err);

  EXPECT_TRUE(result);
}
