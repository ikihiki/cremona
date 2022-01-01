#include "test/include//common.h"
#include <mpack.h>

void dump_device(int id, void *data, void *context){
  mpack_writer_t *writer = (mpack_writer_t*)context;
  device_context_t *device_context = (mpack_writer_t*) data;

  mpack_start_array(writer, 2);
  mpack_write_u32(writer, device_context->pid);
  mpack_write_utf8_cstr_or_nil(writer, device_context->name);
  mpack_finish_array(writer);
}

void dump_devices(crmna_t *crmna, mpack_writer_t *writer){
  int length = get_length(crmna->devices);
  mpack_start_array(writer, length);
  for_each_id_map(crmna->devices, &dump_device, writer);
  mpack_finish_array(writer);
}




bool dump_state(crmna_t *crmna, void *buff, size_t size){
  mpack_writer_t writer;
  mpack_writer_init(&writer, buff, size);

  dump_devices(crmna, &writer);

  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return false;
  }

  return true;
}

void dump_packet(int num, packet_t *packet, mpack_writer_t *writer){
  mpack_start_array(writer, 3);
  mpack_write_u32(writer, packet->offset);
  mpack_write_u32(writer, packet->length);
  mpack_write_bin(writer, packet->data, packet->length);
  mpack_finish_array(writer);
}

void dump_stream(mpack_writer_t *writer, stream_t *stream){
  mpack_start_array(writer, 3);
  mpack_write_u32(writer, stream->stream_id);
  size_t reciv_count = get_length(stream->recive_buffer);
  mpack_start_array(writer, reciv_count);
  for_each_id_map(stream->recive_buffer, &dump_packet, writer);
  mpack_finish_array(writer);
  size_t send_count = get_length(stream->send_buffer);
  mpack_start_array(writer, send_count);
  for_each_id_map(stream->send_buffer, &dump_packet, writer);
  mpack_finish_array(writer);
  mpack_finish_array(writer);
}

void dump_connection(int id, connection_t *connection, mpack_writer_t *writer){

    size_t stream_count = get_length(connection->streams);  
    mpack_start_map(writer, stream_count);
    mpack_write_kv()



  mpack_start_array(writer, send_count);
  for_each_id_map(stream->send_buffer, &dump_packet, writer);
  mpack_finish_array(writer);
  mpack_finish_array(writer);
}


size_t dump_socket(socket_t *socket, char *buff, size_t size){
  mpack_writer_t writer;
  mpack_writer_init(&writer, buff, size);
  size_t conn_len = get_length(socket->connections);
  mpack_start_array(&writer, conn_len);





  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return false;
  }

  return true;
}