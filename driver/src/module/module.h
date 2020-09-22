#ifndef __CREMONA_MODULE__
#define __CREMONA_MODULE__
#include "../cremona/central_store.h"

store_t *create_store(unsigned int miner_min, unsigned int miner_max,
                      const char *driver_name);
void destroy_store(store_t *store);

#endif