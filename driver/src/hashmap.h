#ifndef CRMNA_HASHMAP__
#define CRMNA_HASHMAP__

#ifdef CRMNA_TEST
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#else
#include <linux/types.h>
#endif

typedef struct list_element  {
    uint64_t key;
    list_element_t *previous_element; // is nil for the first item in list
    list_element_t *next_element;     // is nil for the last item in list
    void *value;
    bool deleted; // marks the item as deleting or deleted
} list_element_t;

#endif