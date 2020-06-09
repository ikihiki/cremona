#ifndef CRMNA_COMMON_HEADER
#define CRMNA_COMMON_HEADER

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/types.h>
#else
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#endif

typedef struct crmna_err {
  char *error_msg;
  size_t error_msg_len;
} crmna_err_t;

typedef struct crmna_buf {
  char *buf;
  size_t buf_size;
} crmna_buf_t;

#endif