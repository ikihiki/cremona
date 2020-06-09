#include "id_mapper.h"

bool id_mapper_iterator_next(id_mapper_iterator_ref_t *ref,
                                    crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->next == NULL) {
    err->error_msg = "function pointer \"next\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->next(ref->obj, err);
}

void *id_mapper_iterator_get_value(id_mapper_iterator_ref_t *ref,
                                          crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  if (ref->interface->get_value == NULL) {
    err->error_msg = "function pointer \"get_value\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  return ref->interface->get_value(ref->obj, err);
}

bool id_mapper_iterator_free(id_mapper_iterator_ref_t *ref,
                                    crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->free == NULL) {
    err->error_msg = "function pointer \"free\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->free(ref->obj, err);
}

bool id_mapper_add_get_id(id_mapper_ref_t *ref, void *target,
                                 crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->add_get_id == NULL) {
    err->error_msg = "function pointer \"add_get_id\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->add_get_id(ref->obj, target, err);
}

void *id_mapper_find(id_mapper_ref_t *ref, int id, crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  if (ref->interface->find == NULL) {
    err->error_msg = "function pointer \"find\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  return ref->interface->find(ref->obj, id, err);
}

bool id_mapper_remove(id_mapper_ref_t *ref, int id, crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->remove == NULL) {
    err->error_msg = "function pointer \"remove\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->remove(ref->obj, id, err);
}

bool id_mapper_get_iterator(id_mapper_ref_t *ref,
                                   id_mapper_iterator_ref_t *iterator,
                                   crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->get_iterator == NULL) {
    err->error_msg = "function pointer \"get_iterator\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->get_iterator(ref->obj, iterator, err);
}

bool id_mapper_free(id_mapper_ref_t *ref, crmna_err_t *err) {
  if (ref == NULL) {
    err->error_msg = "reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    err->error_msg = "interface is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    err->error_msg = "interface reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->free == NULL) {
    err->error_msg = "function pointer \"free\" is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->free(ref->obj, err);
}

bool create_id_mapper(id_mapper_factory_t *factory, id_mapper_ref_t *ref,
                             crmna_err_t *err) {
  if (factory == NULL) {
    err->error_msg = "factory is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref == NULL) {
    err->error_msg = "output target reference is null";
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  ref->interface = NULL;
  ref->obj = NULL;

  return factory->create_id_mapper(ref, err);
}