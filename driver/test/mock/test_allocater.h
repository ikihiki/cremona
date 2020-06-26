#pragma once
#include "interfaces/allocator.h"
#include <gmock/gmock.h>

class test_allocater {
public:
  allocator_ref get_ref();
  ~test_allocater() {}
  virtual void *allocate(size_t size) = 0;
  virtual void free(void *ptr) = 0;

private:
  static void *allocate(void *obj, size_t size);
  static void free(void *obj, void *ptr);

  static allocator interface;
};

class test_allocater_mock : public test_allocater {
public:
  MOCK_METHOD1(allocate, void *(size_t size));
  MOCK_METHOD1(free, void(void *ptr));
};