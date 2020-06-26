#include "allocator.h"

void *allocator_allocate(allocator_ref *ref, size_t size){
if (ref == NULL) {
  return NULL;
}
  if (ref->interface == NULL) {
    return NULL;
  }
  if (ref->interface->allocate == NULL) {
    return NULL;
  }
  if (ref->obj == NULL) {
    return NULL;
  }
  return ref->interface->allocate(ref->obj, size);
}
void allocator_free(allocator_ref *ref, void *ptr)
{
  if (ref == NULL) {
    return ;
  }
  if (ref->interface == NULL) {
    return ;
  }
  if (ref->interface->free == NULL) {
    return ;
  }

  ref->interface->free(ref->obj, ptr);
}
