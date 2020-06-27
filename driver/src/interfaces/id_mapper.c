#include "id_mapper.h"

bool id_mapper_iterator_next(id_mapper_iterator_ref *ref, crmna_err_t *err) {
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->next == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"next\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->next(ref->obj, err);
}

void *id_mapper_iterator_get_value(id_mapper_iterator_ref *ref,
                                   crmna_err_t *err) {
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  if (ref->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  if (ref->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  if (ref->interface->get_value == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"get_value\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  return ref->interface->get_value(ref->obj, err);
}

void id_mapper_iterator_free(id_mapper_iterator_ref *ref) {
  ref->interface->free(ref->obj);
  ref->interface = NULL;
  ref->obj = NULL;
}

bool id_mapper_add_get_id(id_mapper_ref *ref, void *target, int *id,  crmna_err_t *err) {
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->add_get_id == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"add_get_id\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->add_get_id(ref->obj, target, id, err);
}

void *id_mapper_find(id_mapper_ref *ref, int id, crmna_err_t *err) {
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  if (ref->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  if (ref->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  if (ref->interface->find == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"find\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return NULL;
  }
  return ref->interface->find(ref->obj, id, err);
}

bool id_mapper_replace(id_mapper_ref *ref, int id, void *new_one,
                       crmna_err_t *err){
  return id_mapper_replace_and_get_entry(ref, id, new_one, NULL, err);
}


bool id_mapper_replace_and_get_entry(id_mapper_ref *ref, int id, void *new_one,
                                     void **replaced_entry, crmna_err_t *err){
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->replace == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
             "function pointer \"replace\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->replace(ref->obj, id,new_one, replaced_entry, err);
}
                                     
bool id_mapper_remove(id_mapper_ref *ref, int id, crmna_err_t *err){
  return id_mapper_remove_and_get_entry(ref, id, NULL, err);
}

bool id_mapper_remove_and_get_entry(id_mapper_ref *ref, int id,
                                    void **removed_entry, crmna_err_t *err) {
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->remove == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"remove\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->remove(ref->obj, id, removed_entry, err);
}

bool id_mapper_get_iterator(id_mapper_ref *ref,
                            id_mapper_iterator_ref *iterator,
                            crmna_err_t *err) {
  if (ref == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->obj == NULL) {
    snprintf(err->error_msg, err->error_msg_len, "interface reference is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  if (ref->interface->get_iterator == NULL) {
    snprintf(err->error_msg, err->error_msg_len,
            "function pointer \"get_iterator\" is null");
    err->error_msg_len = strlen(err->error_msg);
    return false;
  }
  return ref->interface->get_iterator(ref->obj, iterator, err);
}

void id_mapper_free(id_mapper_ref *ref) {
  ref->interface->free(ref->obj);
  ref->interface = NULL;
  ref->obj = NULL;
}

void clear_id_mapper_ref(id_mapper_ref *ref) {
  ref->interface = NULL;
  ref->obj = NULL;
}
