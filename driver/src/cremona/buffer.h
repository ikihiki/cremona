#ifndef __CRMNA_BUFFER_HEADER__
#define __CRMNA_BUFFER_HEADER__
#include <stddef.h>

typedef struct crmna_buf {
  char *buf;
  size_t buf_size;
  size_t used_size;
} crmna_buf_t;

#define DEFINE_CRMNA_BUF(name, size)                                           \
  char name##_buffer_content[size];                                            \
  crmna_buf_t name = {                                                         \
      .buf = name##_buffer_content, .buf_size = size, .used_size = 0};

#define DEFINE_CRMNA_BUF_FROM_MEMORY(name, address, size)                      \
  crmna_buf_t name = {.buf = address, .buf_size = size, .used_size = size}

#endif