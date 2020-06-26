#include "id_mapper_factory.h"

bool create_id_mapper_range(id_mapper_factory_ref *factory, id_mapper_ref *ref,
                            int min, int max, crmna_err_t *err) {
  if (factory == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (factory->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (factory->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (factory->interface->create_id_mapper == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"create_id_mapper\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }

  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "output target reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  ref->interface = NULL;
  ref->obj = NULL;

  return factory->interface->create_id_mapper(factory->obj, ref, min, max, err);
}

bool create_id_mapper(id_mapper_factory_ref *factory, id_mapper_ref *ref,
                      crmna_err_t *err) {
  return create_id_mapper_range(factory, ref, 0, INT_MAX, err);
}