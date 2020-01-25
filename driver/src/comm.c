#include "mpack.h"
#include <linux/kernel.h>

#include "header.h"

int getNameFromCreateAccountMessage(const char *data, size_t length, char *dest,
                                    size_t max_length) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[10];
  mpack_tree_init_pool(&tree, data, length, nodes, 10);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_copy_utf8_cstr(mpack_node_array_at(root, 0), dest, max_length);
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    printk(KERN_ERR "An error occurred decoding the data!\n");
    return -1;
  }
  return 0;
}

int writeCreateAccountResultMessage(int result, const char *name, char *dest,
                                    size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 2);
  mpack_write_i32(&writer, result);
  printk(KERN_INFO "name is %s \n", name);
  mpack_write_utf8_cstr_or_nil(&writer, name);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    printk(KERN_ERR "An error occurred encoding the data!\n");
    return -1;
  }

  return mpack_writer_buffer_used(&writer);
}

/*

*/

int serialize_new_toot(const struct new_toot *data, char *dest,
                       size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 1);
  mpack_start_array(&writer, 16);
  for (int i = 0; i < 16; i++) {
    mpack_write_u8(&writer, data->uuid[i]);
  }
  mpack_finish_array(&writer);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    printk(KERN_ERR "An error occurred encoding the data!\n");
    return -1;
  }

  return mpack_writer_buffer_used(&writer);
}

int deserialize_new_toot_result(const char *data, size_t length,
                                new_toot_result_t *result) {
  mpack_tree_t tree;
  mpack_node_data_t nodes[50];
  mpack_tree_init_pool(&tree, data, length, nodes, 50);
  mpack_tree_parse(&tree);
  mpack_node_t root = mpack_tree_root(&tree);
  mpack_node_t uuid = mpack_node_array_at(root, 0);
  for (int i = 0; i < 16; i++) {
    result->uuid[i] = mpack_node_u8(mpack_node_array_at(uuid, i));
  }
  result->result = mpack_node_int(mpack_node_array_at(root, 1));
  if (mpack_tree_destroy(&tree) != mpack_ok) {
    printk(KERN_ERR
           "Cremona: %s: An error occurred decoding the data!, because %d\n",
           __func__, tree.error);
    return -1;
  }
  return 0;
}

int serialize_add_toot_str(const add_toot_str_t *data, char *dest,
                           size_t max_length) {
  mpack_writer_t writer;
  mpack_writer_init(&writer, dest, max_length);
  mpack_start_array(&writer, 2);
  mpack_start_array(&writer, 16);
  for (int i = 0; i < 16; i++) {
    mpack_write_u8(&writer, data->uuid[i]);
  }
  mpack_finish_array(&writer);
  mpack_write_utf8_cstr_or_nil(&writer, data->text);
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    printk(KERN_ERR
           "Cremona: %s: An error occurred encoding the data!, because %d\n",
           __func__, writer.error);
    return -1;
  }
  return mpack_writer_buffer_used(&writer);
}

  int deserialize_add_toot_str_result(const char *data, size_t length,
                                      add_toot_str_result_t *result) {
    mpack_tree_t tree;
    mpack_node_data_t nodes[50];
    mpack_tree_init_pool(&tree, data, length, nodes, 50);
    mpack_tree_parse(&tree);
    mpack_node_t root = mpack_tree_root(&tree);
    mpack_node_t uuid = mpack_node_array_at(root, 0);
    for (int i = 0; i < 16; i++) {
      result->uuid[i] = mpack_node_u8(mpack_node_array_at(uuid, i));
    }
    result->result = mpack_node_int(mpack_node_array_at(root, 1));
    if (mpack_tree_destroy(&tree) != mpack_ok) {
      printk(KERN_ERR
             "Cremona: %s: An error occurred decoding the data!, because %d\n",
             __func__, tree.error);
      return -1;
    }
    return 0;
  }

  int serialize_send_toot(const send_toot_t *data, char *dest,
                          size_t max_length) {
    mpack_writer_t writer;
    mpack_writer_init(&writer, dest, max_length);
    mpack_start_array(&writer, 1);
    mpack_start_array(&writer, 16);
    for (int i = 0; i < 16; i++) {
      mpack_write_u8(&writer, data->uuid[i]);
    }
    mpack_finish_array(&writer);
    mpack_finish_array(&writer);
    if (mpack_writer_destroy(&writer) != mpack_ok) {
      printk(KERN_ERR
             "Cremona: %s: An error occurred encoding the data!, because %d\n",
             __func__, writer.error);
      return -1;
    }

    return mpack_writer_buffer_used(&writer);
  }
  int deserialize_send_toot_result(const char *data, size_t length,
                                   send_toot_result_t *result) {
    mpack_tree_t tree;
    mpack_node_data_t nodes[50];
    mpack_tree_init_pool(&tree, data, length, nodes, 50);
    mpack_tree_parse(&tree);
    mpack_node_t root = mpack_tree_root(&tree);
    mpack_node_t uuid = mpack_node_array_at(root, 0);
    for (int i = 0; i < 16; i++) {
      result->uuid[i] = mpack_node_u8(mpack_node_array_at(uuid, i));
    }
    result->result = mpack_node_int(mpack_node_array_at(root, 1));
    if (mpack_tree_destroy(&tree) != mpack_ok) {
      printk(KERN_ERR
             "Cremona: %s: An error occurred decoding the data!, because %d\n",
             __func__, tree.error);
      return -1;
    }
    return 0;
  }
