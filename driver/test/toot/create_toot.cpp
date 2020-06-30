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

TEST(toot, create_toot_fail_allocation){

}

TEST(toot, create_toot_fail_create_locker) {}

TEST(toot, create_toot_fail_create_waiter) {}

TEST(toot, create_toot_success) {}