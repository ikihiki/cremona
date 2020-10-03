#include <gtest/gtest.h>
#include "buffer.h"

TEST(buffer, define_buffer){
    DEFINE_CRMNA_BUF(buf, 10)
    EXPECT_EQ(buf.buf_size, 10);
    EXPECT_EQ(buf.used_size, 0);
    EXPECT_EQ(buf.buf, buf_buffer_content);
}