#ifndef __CREMONA_NET_LINK_COMMUNICATOR__
#define __CREMONA_NET_LINK_COMMUNICATOR__

#include "../cremona/commuinicator.h"
#include "../cremona/central_store.h"

bool create_communicator_and_register_store(int port, store_t *store,
                                            communicator_ref_t *ref);

#endif