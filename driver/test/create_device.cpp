#include "central_store.h"
#include "mock_store.h"
#include "mock_communicator.h"
#include "message.h"
#include <cstdarg>
#include <cstdlib>
#include <gtest/gtest.h>

using testing::_;
using testing::DoAll;
using testing::Pointee;
using testing::Return;
using testing::SetArgPointee;
using testing::SetArgReferee;

TEST(device, create_action_from_create_device_message)
{
  char *name = "test_device";
  unsigned char message[] = {0x92, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
                             0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x0d};
  DEFINE_CRMNA_BUF_FROM_MEMORY(data, (char*)message, sizeof(message));

  crmna_err_t error;
  error.curret_line = 0;
  action_t action;
  auto result = create_action_from_message(CRMNA_CREATE_DEVICE, 10, &data,
                                           &action, &error);

  EXPECT_TRUE(result);
  EXPECT_FALSE(HAS_ERROR(&error));
  EXPECT_EQ(action.type, CREATE_DEVICE);
  EXPECT_EQ(action.payload.create_device.pid, 10);
  EXPECT_EQ(action.payload.create_device.uid, 13);
  EXPECT_STREQ(action.payload.create_device.name, name);
}

TEST(device, create_device) {
  DEFINE_TEST_STORE
  crmna_err_t error;
  error.curret_line = 0;
  action_t action = {.type = CREATE_DEVICE,
                     .payload = {.create_device = {.pid = 10, .uid = 13}}};
  strcpy(action.payload.create_device.name, "test_device");

  EXPECT_CALL(mock_store, add_device(10, 13, action.payload.create_device.name,
                                     _, &error))
      .WillOnce(DoAll(SetArgPointee<3>(1), Return(true)));

  EXPECT_CALL(mock_store, attach_device_class(1, &error))
      .WillOnce(Return(true));

  test_communicator_mock com;
  EXPECT_CALL(mock_store, get_communicator())
    .WillOnce(Return(com.get_mock_ref()));

  EXPECT_CALL(mock_store, set_device_ready(1)).WillOnce(Return());

  auto result = dispatch(&store, &action, &error);
  EXPECT_TRUE(result);
  EXPECT_FALSE(HAS_ERROR(&error));
}

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

// TEST(device_manager, create_device_fail_create_callback) {
//   cremona_device_manager device_manager;
//   create_device_manager(&device_manager);
//   device_manager.config.device_collbacks.create_device =
//       &create_device_mock::create_device_err;
//   init_device_manager(&device_manager);
//   char *name = "test_device";
//   unsigned char message[] = {0x91, 0xab, 0x74, 0x65, 0x73, 0x74, 0x5f,
//                              0x64, 0x65, 0x76, 0x69, 0x63, 0x65};
//   char error_msg[100];
//   crmna_err_t err = {.error_msg = error_msg,
//                      .error_msg_len = sizeof(error_msg)};

//   int result = reciveMessage(&device_manager, 100, CRMNA_CREATE_DEVICE,
//                              (char *)message, sizeof(message), &err);

//   EXPECT_FALSE(result);
//   EXPECT_STREQ("Cannot create device. pid: 100", error_msg);
//   destroy_device_manager(&device_manager);
// }