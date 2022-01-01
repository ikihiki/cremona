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


size_t dump_socket(socket_t *socket){
  
}