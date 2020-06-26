#ifndef CRMNA_ALLOCATOR_HEADER
#define CRMNA_ALLOCATOR_HEADER
#include "../common.h"
typedef struct {
  void *(*allocate)(void *obj, size_t size);
  void (*free)(void *obj, void *ptr);
} allocator;

typedef struct {
  allocator *interface;
  void *obj;
} allocator_ref;

void *allocator_allocate(allocator_ref *ref, size_t size);
void allocator_free(allocator_ref *ref, void *ptr);

#endif