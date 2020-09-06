#ifdef __KERNEL__
#include <linux/idr.h>
#include <linux/kernel.h>
#include "central_store.h"


typedef struct store {
    struct idr devices;
    struct idr toots;
    struct idr elements;
} store_t;

store_t *create_store(){
    store_t *store = kzalloc(sizeof(store_t));
    idr_init(store->devices);
    return store;
}

void destroy_store(store_t *store){

}

bool add_device(store_t *store, int pid, int uid, char *name, int *id,
                crmna_err_t *err){

                }
bool attach_device_class(store_t *store, int device_id, crmna_err_t *err);

#endif