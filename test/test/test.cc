#include "mpack.h"
#include <gtest/gtest.h>
#include <map>
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