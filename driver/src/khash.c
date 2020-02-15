#ifdef __KERNEL__

#include <linux/slab.h>
#include <linux/types.h>

#include "khash.h"

void *realloc(void *ptr, size_t size) {
  void *new_ptr = kmalloc(size, GFP_KERNEL);
  memcpy(new_ptr, ptr, ksize(ptr));
  kfree(ptr);
  return new_ptr;
}

#endif