#include "test_allocater.h"
#include <cstdlib>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

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

test_allocater_mock::test_allocater_mock() {
  ON_CALL(*this, allocate(sizeof(cremona_device_t)))
      .WillByDefault(Invoke([this](size_t) {
        if (this->next_device == 10) {
          throw std::runtime_error("overflow device");
        }
        auto result = &(this->devices[this->next_device]);
        this->next_device++;
        return (void*)result;
      }));
  ON_CALL(*this, allocate(sizeof(cremona_toot_t)))
      .WillByDefault(Invoke([this](size_t) {
        if (this->next_toot == 10) {
          throw std::runtime_error("overflow device");
        }
        auto result = &(this->toots[this->next_toot]);
        this->next_toot++;
        return (void *)result;
      }));
}