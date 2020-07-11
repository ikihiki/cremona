#include "common.h"
#include <gtest/gtest.h>

TEST(json_out, test) {
  EXPECT_EQ(JSON_OUT(key1, value1), "{\"key1\": \"value1\"}");

  EXPECT_EQ(JSON_OUT(key1, value1, key2, value2),
            "{\"key1\": \"value1\", \"key2\": \"value2\"}");
}
