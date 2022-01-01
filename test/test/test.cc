#include "mpack.h"
#include <cstddef>
#include <gtest/gtest.h>
#include <map>
#include <stddef.h>
#include <tuple>
#include <utility>

extern "C" {
#include "test/include/common.h"
}

struct socket_context {
  std::map<int, std::tuple<void *>> idr;
};

socket_context_t *create_socket_context() { return new socket_context_t(); }
void destroy_socket_context(socket_context_t *context) { delete context; }
int allocate_id(socket_context_t *context, void *data) {
  int next_id = context->idr.size();
  context->idr.insert(std::make_pair(next_id, std::make_tuple(data)));
  return next_id;
}
void *remove_id(socket_context_t *context, int id) {
  auto data = std::get<0>(context->idr.at(id));
  context->idr.erase(id);
  return data;
}
void update_id(socket_context_t *context, int id,
               void (*func)(void *data, void *capture), void *capture) {
  auto data = std::get<0>(context->idr.at(id));
  func(data, capture);
}

void test_func(void *data, void *capture) { *(int *)data = *(int *)capture; }

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
  int data = 10;
  auto context = create_socket_context();

  auto id = allocate_id(context, &data);
  int update = 20;
  update_id(context, id, &test_func, &update);
  remove_id(context, id);
  EXPECT_EQ(data, 20);
  destroy_socket_context(context);
}

size_t create_conn_device_message(char *buffer, size_t size){
  mpack_writer_t writer;
  mpack_writer_init(&writer, buffer, size);
  mpack_start_array(&writer, 3);
  mpack_write_u32(&writer, 1);
  mpack_write_u32(&writer, 1);
  mpack_start_array(&writer, 1);
  mpack_write_utf8_cstr_or_nil(&writer, "test");
  mpack_finish_array(&writer);
  mpack_finish_array(&writer);

    if (mpack_writer_destroy(&writer) != mpack_ok) {
    return 0;
  }

  return mpack_writer_buffer_used(&writer);
}


size_t create_end_device_message(char *buffer, size_t size){
  mpack_writer_t writer;
  mpack_writer_init(&writer, buffer, size);
  mpack_start_array(&writer, 3);
  mpack_write_u32(&writer, 3);
  mpack_write_u32(&writer, 4);
  mpack_finish_array(&writer);

  if (mpack_writer_destroy(&writer) != mpack_ok) {
    return 0;
  }
  return mpack_writer_buffer_used(&writer);
}


// TEST(Cremona, StartDevice){
//   crmna_t crmna;
//   create_cremona(&crmna);
  
//   char packet[1000]; 
//   auto packet_size = create_conn_device_message(packet, sizeof(packet));
//   if(packet_size == 0){
//     FAIL();
//   }

// recive_data_to_socket(packet, packet_size);

//   char dump[1000];
//   dump_state(&crmna, dump, sizeof(dump));
//   mpack_tree_t tree;
//   mpack_node_data_t nodes[50];
//   mpack_tree_init_pool(&tree, dump, sizeof(dump), nodes, 50);
//   mpack_tree_parse(&tree);
//   mpack_node_t root = mpack_tree_root(&tree);
//   mpack_node_t devices = mpack_node_array_at(root, 0);
//   mpack_node_t device1 = mpack_node_array_at(devices, 0);
//   mpack_node_t device1_pid = mpack_node_array_at(device1, 0);

//   EXPECT_EQ(mpack_node_u32(device1_pid), 20); 
 
//   auto device1_name = mpack_node_array_at(device1, 1);
//   char name[100];
//   mpack_node_copy_utf8_cstr(device1_name, name, sizeof(name));
//   EXPECT_STREQ(name, "test"); 
// }

// TEST(Cremona, EndDevice){
//   crmna_t crmna;
//   char dump[1000];
//   create_cremona(&crmna);
//   dump_state(&crmna, dump, sizeof(dump));

//   char start_device_message[1000]; 
//   auto start_device_message_len = create_conn_device_message(start_device_message, sizeof(start_device_message));
//   if(start_device_message_len == 0){
//     FAIL();
//   }
//   recive_data_to_socket(start_device_message, start_device_message_len);

//   char packet[1000]; 
//  create_end_device_message(packet);
// recive_data_to_socket(packet, mpack_writer_buffer_used(&writer))


//   mpack_tree_t tree;
//   mpack_node_data_t nodes[50];
//   mpack_tree_init_pool(&tree, dump, data->used_size, nodes, 50);
//   mpack_tree_parse(&tree);
//   mpack_node_t root = mpack_tree_root(&tree);
//   mpack_node_t devices = mpack_node_array_at(root, 0);

// EXPECT_EQ(mpack_node_u32(devices),·20); 
 

// }



// TEST(Cremona, WriteDevice){
//   crmna_t crmna;
//   char dump[1000];
//   create_cremona(&crmna);
//   dump_state(&crmna, dump, sizeof(dump));


//   char packet[1000]; 
//  create_conn_device_message(packet);
// recive_data_to_socket(packet, mpack_writer_buffer_used(&writer))

//   char mess = "aaaaaa";

//   write_device(crmna,mess, sizeof(mess));


//   char packet[1000]; 
//  create_conn_device_message(packet);
// recive_data_to_socket(packet, mpack_writer_buffer_used(&writer))


//   mpack_tree_t tree;
//   mpack_node_data_t nodes[50];
//   mpack_tree_init_pool(&tree, dump, data->used_size, nodes, 50);
//   mpack_tree_parse(&tree);
//   mpack_node_t root = mpack_tree_root(&tree);
//   mpack_node_t devices = mpack_node_array_at(root, 0);

// EXPECT_EQ(mpack_node_u32(devices),·20); 
 

// }