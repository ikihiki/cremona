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

TEST(toot, open_toot_fail_not_open) {
}

TEST(toot, open_toot_fail_destroied) {}

TEST(toot, open_toot_fail_error) {}

TEST(toot, open_toot_add_text_result_wait) {}

TEST(toot, open_toot_write_compleate) {}

TEST(toot, open_toot_close_wait) {}

TEST(toot, open_toot_time_out) {}