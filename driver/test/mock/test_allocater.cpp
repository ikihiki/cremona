#include "test_allocater.h"
#include <cstdlib>

void *test_allocater::allocate(void *obj, size_t size) {
  return ((test_allocater *)obj)->allocate(size);
}
void test_allocater::free(void *obj, void *ptr) {
  ((test_allocater *)obj)->free(ptr);
}

allocator test_allocater::interface = {.allocate = &test_allocater::allocate,
                                       .free = &test_allocater::free};

allocator_ref test_allocater::get_ref() {
  return {.interface = &test_allocater::interface, .obj = this};
}