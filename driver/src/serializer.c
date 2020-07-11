
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
  mpack_node_t uid = mpack_node_array_at(root, 1);
  result->uid = mpack_node_u32(uid);
  if (mpack_tree_destroy(&tree) != mpack_ok)
  {
    return false;
  }
  return true;
}

int serialize_create_device_result(const create_device_result_t *data,
                                   char *dest, size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 1);
  mpack_write_i32(&writer, data->id);
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
  mpack_write_i32(&writer, data->id);
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
  result->id = mpack_node_i32(id);
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return false;
  }
  return true;
}

int serialize_new_toot(const new_toot_t *data, char *dest, size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 3);
  mpack_write_i32(&writer, data->message_id);
  mpack_write_i32(&writer, data->toot_id);
  mpack_write_i32(&writer, data->device_id);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return -1;
  }

  return mpack_writer_buffer_used(&writer);
}

bool deserialize_new_toot_result(const char *data, size_t length,
                                new_toot_result_t *message) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data, length, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t message_id = mpack_node_array_at(root, 0);
  message->message_id = mpack_node_i32(message_id);
  mpack_node_t toot_id = mpack_node_array_at(root, 1);
  message->toot_id = mpack_node_i32(toot_id);
  mpack_node_t device_id = mpack_node_array_at(root, 2);
  message->device_id = mpack_node_i32(device_id);
  mpack_node_t result = mpack_node_array_at(root, 3);
  message->result = mpack_node_bool(result);
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return false;
  }
  return true;
}

int serialize_add_toot_text(const add_toot_text_t *data, char *dest,
                           size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 3);
  mpack_write_i32(&writer, data->toot_id);
  mpack_write_i32(&writer, data->device_id);
  mpack_write_utf8_cstr_or_nil(&writer, data->text);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return -1;
  }
  return mpack_writer_buffer_used(&writer);
}

bool deserialize_add_toot_text_result(const char *data, size_t length,
                                    add_toot_text_result_t *result) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data, length, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t toot_id = mpack_node_array_at(root, 0);
  result->toot_id = mpack_node_i32(toot_id);
  mpack_node_t device_id = mpack_node_array_at(root, 1);
  result->device_id = mpack_node_i32(device_id);
  mpack_node_t result_num = mpack_node_array_at(root, 2);
  result->result = mpack_node_i32(result_num);
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return false;
  }
  return true;
}

int serialize_send_toot(const send_toot_t *data, char *dest,
                        size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 2);
  mpack_write_i32(&writer, data->toot_id);
  mpack_write_i32(&writer, data->device_id);
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
  result->toot_id = mpack_node_i32(toot_id);
  mpack_node_t device_id = mpack_node_array_at(root, 1);
  result->device_id = mpack_node_i32(device_id);
  mpack_node_t result_num = mpack_node_array_at(root, 2);
  result->result = mpack_node_i32(result_num);
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    return false;
  }
  return true;
}
