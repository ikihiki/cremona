#ifndef __CREMONA_HEADER__
#define __CREMONA_HEADER__

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

#include "khash.h"

typedef enum cremona_error_level { ERROR, WARN, INFO } cremona_error_level_t;

#define LOG_IMPL(device_manager, error, fmt, ...)                              \
  (device_manager->config.devicce_manager_callbacks.log(                       \
      error, "Cremona: %s: " fmt "\n", __func__, __VA_ARGS__))

#define LOG_ERROR(device_manager, fmt, ...)                                    \
  LOG_IMPL(device_manager, ERROR, fmt, __VA_ARGS__)

#define LOG_WARN(device_manager, fmt, ...)                                     \
  LOG_IMPL(device_manager, WARN, fmt, __VA_ARGS__)

#define LOG_INFO(device_manager, fmt, ...)                                     \
  LOG_IMPL(device_manager, INFO, fmt, __VA_ARGS__)

#define LOG_AND_WRITE_ERROR(device_manager, crmna_err, fmt, ...)               \
  {                                                                            \
    LOG_ERROR(device_manager, fmt, __VA_ARGS__);                               \
    snprintf(crmna_err->error_msg, crmna_err->error_msg_len, fmt,              \
             __VA_ARGS__);                                                     \
  }

typedef struct cremona_device_manager cremona_device_manager_t;
typedef struct cremona_device cremona_device_t;

typedef struct crmna_err {
  char *error_msg;
  size_t error_msg_len;
} crmna_err_t;

typedef enum toot_state {
  CREANUPED,
  OPEN_RESULT_WAIT,
  OPEND,
  ADD_STRING_RESULT_WAIT,
  CLOSE_RESULT_WAIT,
  DESTROYED,
  TOOT_ERROR
} toot_state_t;

typedef enum toot_wait_type{
  WAIT_OPEN,
  WAIT_WRITE,
  WAIT_CLOSE
} toot_wait_type_t;

typedef struct cremona_toot {
  uint64_t id;
  toot_state_t state;
  cremona_device_t *device;
  int refCount;
} cremona_toot_t;

typedef struct cremona_toot_callbacks {
  void (*lock)(cremona_toot_t *toot);
  void (*unlock)(cremona_toot_t *toot);
  void (*wait)(cremona_toot_t *toot, toot_wait_type_t wait_type);
  void (*notify)(cremona_toot_t *toot);
  cremona_toot_t *(*create_toot)(void);
  bool (*destroy_toot)(cremona_toot_t *toot);
  void (*cleanup_toot)(cremona_toot_t *toot);
} cremona_toot_callbacks_t;

KHASH_MAP_INIT_INT64(toot, cremona_toot_t *)

typedef struct cremona_device {
  char name[50];
  uint64_t id;
  uint32_t pid;
  bool isDestroied;
  int refCount;
  khash_t(toot) * toots;
  cremona_device_manager_t *device_manager;
} cremona_device_t;

typedef struct cremona_device_callbacks {
  void (*lock)(cremona_device_t *device);
  void (*unlock)(cremona_device_t *device);
  cremona_device_t *(*create_device)(char *name);
  bool (*destroy_device)(cremona_device_t *device);
  void (*cleanup_device)(cremona_device_t *device);
} cremona_device_callbacks_t;

typedef struct cremona_device_manager cremona_device_manager_t;

typedef struct cremona_device_manager_callbacks {
  int (*send_message)(uint32_t pid, int type, char *buf, size_t buf_size);
  uint32_t (*get_time)(void);
  uint32_t (*get_rand)(void);
  void (*lock)(cremona_device_manager_t *device_manager);
  void (*unlock)(cremona_device_manager_t *device_manager);
  void (*log)(cremona_error_level_t level, char *fmt, ...);
} cremona_device_manager_callbacks_t;

typedef struct cremona_device_manager_config {
  cremona_device_manager_callbacks_t devicce_manager_callbacks;
  cremona_device_callbacks_t device_collbacks;
  cremona_toot_callbacks_t toot_callbacks;
} cremona_device_manager_config_t;

KHASH_MAP_INIT_INT64(dm, cremona_device_t *)

typedef struct cremona_device_manager {
  cremona_device_manager_config_t config;
  int refCount;
  khash_t(dm) * devices;
} cremona_device_manager_t;

bool init_device_manager(cremona_device_manager_t *device_manager);
void destroy_device_manager(cremona_device_manager_t *device_manager);

bool reciveMessage(cremona_device_manager_t *device_manager, uint32_t pid,
                   int type, char *buf, size_t buf_size, crmna_err_t *error);

void *cremona_malloc(cremona_device_manager_t *device_manager, int size);

cremona_device_t *get_cremona_device(cremona_device_manager_t *device_manager,
                                     uint64_t id);

cremona_toot_t *add_ref_toot(cremona_toot_t *toot);
void release_toot(cremona_toot_t *toot);
cremona_toot_t *open_toot(cremona_device_t *device, bool wait, crmna_err_t *err);
bool close_toot( cremona_toot_t *toot, crmna_err_t *err);

#endif