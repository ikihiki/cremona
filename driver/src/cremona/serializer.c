
#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif
#include "mpack.h"

#include "message.h"

bool serialize_get_stats_result(const get_stats_result_t *data,
                                  crmna_buf_t *dest) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest->buf, dest->buf_size);
  mpack_start_array(&writer, 2);
  mpack_write_i32(&writer, data->total_device_count);
  mpack_write_i32(&writer, data->total_open_toot);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return false;
  }

  dest->used_size = mpack_writer_buffer_used(&writer);
  return true;
}

bool deserialize_create_conn(const crmna_buf_t *data,
                             create_device_t *result) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data->buf, data->used_size, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t name = mpack_node_array_at(root, 0);
  mpack_node_copy_utf8_cstr(name, result->name, sizeof(result->name));
  mpack_node_t uid = mpack_node_array_at(root, 1);
  result->uid = mpack_node_u32(uid);
  if (mpack_tree_destroy(&tree) != mpack_ok)
  {
    return false;
  }
  return true;
}

bool serialize_create_device_result(const create_device_result_t *data,
                                    crmna_buf_t *dest) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest->buf, dest->buf_size);
  mpack_start_array(&writer, 1);
  mpack_write_u32(&writer, data->id);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return false;
  }

  dest->used_size = mpack_writer_buffer_used(&writer);
  return true;
}

bool serialize_destroy_device_result(const destroy_device_result_t *data,
                                     crmna_buf_t *dest) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest->buf, dest->buf_size);
  mpack_start_array(&writer, 1);
  mpack_write_u32(&writer, data->id);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return -1;
  }

  dest->used_size = mpack_writer_buffer_used(&writer);
  return true;
}
bool deserialize_destroy_device(const crmna_buf_t *data,
                                destroy_device_t *result) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data->buf, data->used_size, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t id = mpack_node_array_at(root, 0);
  result->id = mpack_node_u32(id);
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return false;
  }
  return true;
}

bool serialize_new_toot(const new_toot_t *data, crmna_buf_t *dest) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest->buf, dest->buf_size);
  mpack_start_array(&writer, 2);
  mpack_write_u32(&writer, data->toot_id);
  mpack_write_u32(&writer, data->device_id);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return false;
  }

  dest->used_size = mpack_writer_buffer_used(&writer);
  return true;
}

bool deserialize_new_toot_result(const crmna_buf_t *data,
                                 new_toot_result_t *result) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data->buf, data->used_size, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t toot_id = mpack_node_array_at(root, 0);
  result->toot_id = mpack_node_u32(toot_id);
  mpack_node_t device_id = mpack_node_array_at(root, 1);
  result->device_id = mpack_node_u32(device_id);
  mpack_node_t result_num = mpack_node_array_at(root, 2);
  result->result = mpack_node_i32(result_num);
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return false;
  }
  return true;
}

bool serialize_add_toot_text(const add_toot_text_t *data, crmna_buf_t *dest) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest->buf, dest->buf_size);
  mpack_start_array(&writer, 3);
  mpack_write_u32(&writer, data->toot_id);
  mpack_write_u32(&writer, data->device_id);
  mpack_write_utf8_cstr_or_nil(&writer, data->text);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return false;
  }
  dest->used_size = mpack_writer_buffer_used(&writer);
  return true;
}

bool deserialize_add_toot_text_result(const crmna_buf_t *data,
                                      add_toot_text_result_t *result) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data->buf, data->used_size, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t toot_id = mpack_node_array_at(root, 0);
  result->toot_id = mpack_node_u32(toot_id);
  mpack_node_t device_id = mpack_node_array_at(root, 1);
  result->device_id = mpack_node_u32(device_id);
  mpack_node_t result_num = mpack_node_array_at(root, 2);
  result->result = mpack_node_i32(result_num);
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return false;
  }
  return true;
}

bool serialize_send_toot(const send_toot_t *data, crmna_buf_t *dest) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest->buf, dest->buf_size);
  mpack_start_array(&writer, 2);
  mpack_write_u32(&writer, data->toot_id);
  mpack_write_u32(&writer, data->device_id);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return false;
  }

  dest->used_size = mpack_writer_buffer_used(&writer);
  return true;
}

bool deserialize_send_toot_result(const crmna_buf_t *data,
                                  send_toot_result_t *result) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data->buf, data->used_size, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t toot_id = mpack_node_array_at(root, 0);
  result->toot_id = mpack_node_u32(toot_id);
  mpack_node_t device_id = mpack_node_array_at(root, 1);
  result->device_id = mpack_node_u32(device_id);
  mpack_node_t result_num = mpack_node_array_at(root, 2);
  result->result = mpack_node_i32(result_num);
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return false;
  }
  return true;
}
