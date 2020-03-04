
#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif
#include "mpack.h"

#include "message.h"

size_t serialize_get_stats_result(const get_stats_result_t *data, char *dest,
                                  size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 2);
  mpack_write_i32(&writer, data->total_device_count);
  mpack_write_i32(&writer, data->total_open_toot);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return -1;
  }

  return mpack_writer_buffer_used(&writer);
}

bool deserialize_create_conn(const char *data, size_t length,
                             create_device_t *result) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data, length, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t name = mpack_node_array_at(root, 0);
  mpack_node_copy_utf8_cstr(name, result->name, sizeof(result->name));
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return false;
  }
  return true;
}

int serialize_create_device_result(const create_device_result_t *data,
                                   char *dest, size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 1);
  mpack_write_u64(&writer, data->id);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return -1;
  }

  return mpack_writer_buffer_used(&writer);
}

int serialize_destroy_device_result(const destroy_device_result_t *data,
                                    char *dest, size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 1);
  mpack_write_u64(&writer, data->id);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return -1;
  }

  return mpack_writer_buffer_used(&writer);
}
bool deserialize_destroy_device(const char *data, size_t length,
                                destroy_device_t *result) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data, length, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t id = mpack_node_array_at(root, 0);
  result->id = mpack_node_u64(id);
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return false;
  }
  return true;
}

int serialize_new_toot(const new_toot_t *data, char *dest, size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 2);
  mpack_write_u64(&writer, data->toot_id);
  mpack_write_u64(&writer, data->device_id);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return -1;
  }

  return mpack_writer_buffer_used(&writer);
}

bool deserialize_new_toot_result(const char *data, size_t length,
                                new_toot_result_t *result) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data, length, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t toot_id = mpack_node_array_at(root, 0);
  result->toot_id = mpack_node_u64(toot_id);
  mpack_node_t device_id = mpack_node_array_at(root, 1);
  result->device_id = mpack_node_u64(device_id);
  mpack_node_t result_num = mpack_node_array_at(root, 2);
  result->result = mpack_node_i32(result_num);
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return false;
  }
  return true;
}

int serialize_add_toot_str(const add_toot_str_t *data, char *dest,
                           size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 2);
  mpack_write_bin(&writer, (const char *)data->uuid, 16);
  mpack_write_utf8_cstr_or_nil(&writer, data->text);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return -1;
  }
  return mpack_writer_buffer_used(&writer);
}

bool deserialize_add_toot_str_result(const char *data, size_t length,
                                    add_toot_str_result_t *result) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data, length, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t uuid = mpack_node_array_at(root, 0);
  if (mpack_node_bin_size(uuid) != 16) {
    return -1;
  }
  memcpy(result->uuid, mpack_node_bin_data(uuid), 16);
  result->result = mpack_node_int(mpack_node_array_at(root, 1));
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return -1;
  }
  return 0;
}

int serialize_send_toot(const send_toot_t *data, char *dest,
                        size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 2);
  mpack_write_u64(&writer, data->toot_id);
  mpack_write_u64(&writer, data->device_id);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return -1;
  }

  return mpack_writer_buffer_used(&writer);
}

bool deserialize_send_toot_result(const char *data, size_t length,
                                 send_toot_result_t *result) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data, length, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t toot_id = mpack_node_array_at(root, 0);
  result->toot_id = mpack_node_u64(toot_id);
  mpack_node_t device_id = mpack_node_array_at(root, 1);
  result->device_id = mpack_node_u64(device_id);
  mpack_node_t result_num = mpack_node_array_at(root, 2);
  result->result = mpack_node_i32(result_num);
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return false;
  }
  return true;
}
