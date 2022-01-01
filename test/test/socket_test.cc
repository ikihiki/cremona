#include <gtest/gtest.h>
#include <mpack.h>
#include "test/include/common.h"
#include "test/test/test.h"

struct packet_struct{
    uint32_t stream_id;
    uint32_t offset;
    uint32_t size;
    char *data;
};


size_t create_conn_device_message(char *buffer, size_t size){
  char data[1];
  data[0] = 1;
  mpack_writer_t writer;
  mpack_writer_init(&writer, buffer, size);
  mpack_start_array(&writer, 4);
  mpack_write_u32(&writer, 1);
  mpack_write_u32(&writer, 0);
  mpack_write_u32(&writer, 1);
  mpack_write_bin(&writer, data, sizeof(data));
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
    create_conn_device_message(buffer, sizeof(buffer))

}