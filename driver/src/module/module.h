#ifndef __CREMONA_MODULE__
#define __CREMONA_MODULE__
#include "../cremona/central_store.h"

store_t *create_store(uint32_t miner_min, uint32_t miner_max,
                      const char *driver_name, const char *class_name);
void destroy_store(store_t *store);

void printk_err(crmna_err_t *err);

#endif