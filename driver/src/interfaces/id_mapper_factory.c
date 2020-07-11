#include "id_mapper_factory.h"

bool create_id_mapper_range(id_mapper_factory_ref *factory, id_mapper_ref *ref,
                            int min, int max, crmna_err *err) {
  if (factory == NULL) {
    ADD_ERROR(err, "reference is null");

    return false;
  }
  if (factory->interface == NULL) {
    ADD_ERROR(err, "interface is null");

    return false;
  }
  if (factory->obj == NULL) {
    ADD_ERROR(err, "interface reference is null");

    return false;
  }
  if (factory->interface->create_id_mapper == NULL) {
    ADD_ERROR(err, "function pointer \"create_id_mapper\" is null");

    return false;
  }

  if (ref == NULL) {
    ADD_ERROR(err, "output target reference is null");

    return false;
  }
  ref->interface = NULL;
  ref->obj = NULL;

  return factory->interface->create_id_mapper(factory->obj, ref, min, max, err);
}

bool create_id_mapper(id_mapper_factory_ref *factory, id_mapper_ref *ref,
                      crmna_err *err) {
  return create_id_mapper_range(factory, ref, 0, INT_MAX, err);
}