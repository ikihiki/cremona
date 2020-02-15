#ifndef HANDLE___
#define HANDLE___

#ifdef CRMNA_TEST
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#else
#include <linux/types.h>
#endif

#include "khash.h"

typedef struct cremona_toot_t {
  char *name;
  int miner;
  int pid;
  void *container;
} cremona_toot;

typedef struct cremona_toot_callbacks {
  int (*lock)(cremona_toot *device);
  int (*unlock)(cremona_toot *device);
  int (*wait)(cremona_toot *device);
  int (*notify)(cremona_toot *device);
  int (*create_toot)(cremona_toot *toot);
  int (*destroy_toot)(cremona_toot *toot);
} cremona_toot_callbacks_t;

typedef struct cremona_device {
  char name[50];
  uint64_t id;
  int pid;
  void *context;
} cremona_device_t;

typedef struct cremona_device_callbacks {
  int (*lock)(cremona_device_t *device);
  int (*unlock)(cremona_device_t *device);
  bool (*create_device)(cremona_device_t *device);
  bool (*destroy_device)(cremona_device_t *device);
} cremona_device_callbacks_t;

typedef struct cremona_device_manager cremona_device_manager_t;

typedef struct cremona_device_manager_callbacks {
  void *(*allocate)(size_t size);
  void (*deallocate)(void *ptr);
  int (*send_message)(uint32_t pid, int type, char *buf, size_t buf_size);
  uint32_t (*get_time)(void);
  uint32_t (*get_rand)(void);
  void (*lock)(cremona_device_manager_t *device_manager);
  void (*unlock)(cremona_device_manager_t *device_manager);
} cremona_device_manager_callbacks_t;

typedef struct cremona_device_manager_config {
  cremona_device_manager_callbacks_t devicce_manager_callbacks;
  cremona_device_callbacks_t device_collbacks;
  cremona_toot_callbacks_t toot_callbacks;
} cremona_device_manager_config_t;

KHASH_MAP_INIT_INT64(dm, cremona_device_t *)

typedef struct cremona_device_manager {
  cremona_device_manager_config_t config;
  khash_t(dm) *devices;
  void *context;
} cremona_device_manager_t;

int initDeviceManager(cremona_device_manager_t *device_manager);

bool reciveMessage(cremona_device_manager_t *device_manager, uint32_t pid,
                   int type, char *buf, size_t buf_size);

void *cremona_malloc(cremona_device_manager_t *device_manager, int size);




#endif