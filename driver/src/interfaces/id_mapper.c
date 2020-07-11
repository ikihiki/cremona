#include "id_mapper.h"

bool id_mapper_iterator_next(id_mapper_iterator_ref *ref, crmna_err *err) {
  if (ref == NULL) {
    ADD_ERROR(err, "reference is null");

    return false;
  }
  if (ref->interface == NULL) {
    ADD_ERROR(err, "interface is null");

    return false;
  }
  if (ref->obj == NULL) {
    ADD_ERROR(err, "interface reference is null");

    return false;
  }
  if (ref->interface->next == NULL) {
    ADD_ERROR(err, "function pointer \"next\" is null");

    return false;
  }
  return ref->interface->next(ref->obj, err);
}

void *id_mapper_iterator_get_value(id_mapper_iterator_ref *ref,
                                   crmna_err *err) {
  if (ref == NULL) {
    ADD_ERROR(err, "reference is null");

    return NULL;
  }
  if (ref->interface == NULL) {
    ADD_ERROR(err, "interface is null");

    return NULL;
  }
  if (ref->obj == NULL) {
    ADD_ERROR(err, "interface reference is null");

    return NULL;
  }
  if (ref->interface->get_value == NULL) {
    ADD_ERROR(err, "function pointer \"get_value\" is null");

    return NULL;
  }
  return ref->interface->get_value(ref->obj, err);
}

void id_mapper_iterator_free(id_mapper_iterator_ref *ref) {
  if (ref == NULL) {
    return;
  }
  if (ref->interface != NULL) {
    ref->interface->free(ref->obj);
  }
  ref->interface = NULL;
  ref->obj = NULL;
}

bool id_mapper_add_get_id(id_mapper_ref *ref, void *target, int *id,
                          crmna_err *err) {
  if (ref == NULL) {
    ADD_ERROR(err, "reference is null");

    return false;
  }
  if (ref->interface == NULL) {
    ADD_ERROR(err, "interface is null");

    return false;
  }
  if (ref->obj == NULL) {
    ADD_ERROR(err, "interface reference is null");

    return false;
  }
  if (ref->interface->add_get_id == NULL) {
    ADD_ERROR(err, "function pointer \"add_get_id\" is null");

    return false;
  }
  return ref->interface->add_get_id(ref->obj, target, id, err);
}

void *id_mapper_find(id_mapper_ref *ref, int id, crmna_err *err) {
  if (ref == NULL) {
    ADD_ERROR(err, "reference is null");

    return NULL;
  }
  if (ref->interface == NULL) {
    ADD_ERROR(err, "interface is null");

    return NULL;
  }
  if (ref->obj == NULL) {
    ADD_ERROR(err, "interface reference is null");

    return NULL;
  }
  if (ref->interface->find == NULL) {
    ADD_ERROR(err, "function pointer \"find\" is null");

    return NULL;
  }
  return ref->interface->find(ref->obj, id, err);
}

bool id_mapper_replace(id_mapper_ref *ref, int id, void *new_one,
                       crmna_err *err) {
  return id_mapper_replace_and_get_entry(ref, id, new_one, NULL, err);
}

bool id_mapper_replace_and_get_entry(id_mapper_ref *ref, int id, void *new_one,
                                     void **replaced_entry, crmna_err *err) {
  if (ref == NULL) {
    ADD_ERROR(err, "reference is null");

    return false;
  }
  if (ref->interface == NULL) {
    ADD_ERROR(err, "interface is null");

    return false;
  }
  if (ref->obj == NULL) {
    ADD_ERROR(err, "interface reference is null");

    return false;
  }
  if (ref->interface->replace == NULL) {
    ADD_ERROR(err, "function pointer \"replace\" is null");

    return false;
  }
  return ref->interface->replace(ref->obj, id, new_one, replaced_entry, err);
}

bool id_mapper_remove(id_mapper_ref *ref, int id, crmna_err *err) {
  return id_mapper_remove_and_get_entry(ref, id, NULL, err);
}

bool id_mapper_remove_and_get_entry(id_mapper_ref *ref, int id,
                                    void **removed_entry, crmna_err *err) {
  if (ref == NULL) {
    ADD_ERROR(err, "reference is null");

    return false;
  }
  if (ref->interface == NULL) {
    ADD_ERROR(err, "interface is null");

    return false;
  }
  if (ref->obj == NULL) {
    ADD_ERROR(err, "interface reference is null");

    return false;
  }
  if (ref->interface->remove == NULL) {
    ADD_ERROR(err, "function pointer \"remove\" is null");

    return false;
  }
  return ref->interface->remove(ref->obj, id, removed_entry, err);
}

bool id_mapper_get_iterator(id_mapper_ref *ref,
                            id_mapper_iterator_ref *iterator, crmna_err *err) {
  if (ref == NULL) {
    ADD_ERROR(err, "reference is null");

    return false;
  }
  if (ref->interface == NULL) {
    ADD_ERROR(err, "interface is null");

    return false;
  }
  if (ref->obj == NULL) {
    ADD_ERROR(err, "interface reference is null");

    return false;
  }
  if (ref->interface->get_iterator == NULL) {
    ADD_ERROR(err, "function pointer \"get_iterator\" is null");

    return false;
  }
  return ref->interface->get_iterator(ref->obj, iterator, err);
}

void id_mapper_free(id_mapper_ref *ref) {
  if (ref == NULL) {
    return;
  }
  if (ref->interface != NULL) {
    ref->interface->free(ref->obj);
  }
  ref->interface = NULL;
  ref->obj = NULL;
}

void clear_id_mapper_ref(id_mapper_ref *ref) {
  ref->interface = NULL;
  ref->obj = NULL;
}
