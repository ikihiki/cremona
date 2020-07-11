#include "../mock/test_allocater.h"
#include "../mock/test_communicator.h"
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
using ::testing::EndsWith;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Pointee;
using ::testing::Return;

TEST(device_manager, init_device_manager_fail_create_communicator) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  EXPECT_CALL(com_factory_mock, create_communicator(_, _, _))
      .WillOnce(
          DoAll(Invoke([](communicate_t *, communicator_ref *, crmna_err *err) {
                  ADD_ERROR(err, "error_communicator");
                }),
                Return(false)));
  test_locker_factory_mock locker_factory_mock;
  test_id_mapper_factory_mock id_factory_mock;
  bool result = init_device_manager(&device_manager, 0, 0,
                                    com_factory_mock.get_mock_factory(),
                                    locker_factory_mock.get_mock_factory(),
                                    id_factory_mock.get_mock_factory(), NULL,
                                    NULL, &test_logger::logger_ref, NULL, &err);

  EXPECT_FALSE(result);
  EXPECT_THAT(err.error_msg[0], EndsWith("error_communicator"));
  out_err(&err);
}

TEST(device_manager, init_device_manager_fail_create_locker) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_mock;
  EXPECT_CALL(locker_mock, create_locker(_, _))
      .WillOnce(DoAll(Invoke([](locker_ref *, crmna_err *err) {
                        ADD_ERROR(err, "error_locker");
                      }),
                      Return(false)));
  auto locker_factory = locker_mock.get_factory();

  bool result =
      init_device_manager(&device_manager, 0, 0, &com_factory, &locker_factory,
                          NULL, NULL, NULL, NULL, NULL, &err);

  EXPECT_FALSE(result);
  EXPECT_THAT(err.error_msg[0], EndsWith("error_locker"));
  out_err(&err);
}

TEST(device_manager, init_device_manager_fail_create_id_mapper) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);
  test_communicator_factory_mock com_factory_mock;
  auto com_factory = com_factory_mock.get_factory();

  test_locker_factory_mock locker_factory_mock;
  auto locker_factory = locker_factory_mock.get_factory();

  test_id_mapper_factory_mock id_mock;
  EXPECT_CALL(id_mock, create_id_mapper(_, _, _, _))
      .WillOnce(Invoke([](id_mapper_ref *, int, int, crmna_err *err) {
        ADD_ERROR(err, "error_id_mapper");
        return false;
      }));
  auto id_mapper_factory = id_mock.get_factory();

  bool result =
      init_device_manager(&device_manager, 0, 0, &com_factory, &locker_factory,
                          &id_mapper_factory, NULL, NULL, NULL, NULL, &err);

  EXPECT_FALSE(result);
  EXPECT_THAT(err.error_msg[0], EndsWith("error_id_mapper"));
  out_err(&err);
}

TEST(device_manager, init_device_manager_success) {
  cremona_device_manager_t device_manager;
  DEFINE_ERROR(err);

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
  EXPECT_FALSE(HAS_ERROR(&err));
  out_err(&err);
}
