#include <gtest/gtest.h>
#include <mpack.h>
#include "test/include/common.h"
#include "test/test/test.h"

struct packet_struct{
    uint8_t packet_type;

    uint32_t stream_id;
    uint32_t offset;
    uint32_t size;
    bool is_finish;
    char *data;
};

struct conn_open{
    uint8_t packet_type = 1;
    uint32_t flag = 0;
};

struct con_close {
    uint8_t packet_type = 2;
    uint32_t flag = 0;
};

struct send_data {
    uint8_t packet_type = 3;
    uint32_t stream_id;
    uint32_t offset;
    uint32_t size;
    bool is_finish;
    char *data;
};



size_t create_conn_message(char *buffer, size_t size){
  mpack_writer_t writer;
  mpack_writer_init(&writer, buffer, size);
  mpack_start_array(&writer, 2);
  mpack_write_u32(&writer, 1);
  mpack_start_array(&writer, 1);
  mpack_write_u32(&writer, 0);
  mpack_finish_array(&writer);
  mpack_finish_array(&writer);

  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return 0;
  }

  return mpack_writer_buffer_used(&writer);
}


TEST(socket, connect){
    socket_context_t context;
    socket socket;
    init_socket(&socket, &context);

    char buffer[50];
    auto mess_len = create_conn_message(buffer, sizeof(buffer));
    if(mess_len == 0){
        FAIL();
    }

    recive_data_to_socket(&socket, buffer, mess_len);
}