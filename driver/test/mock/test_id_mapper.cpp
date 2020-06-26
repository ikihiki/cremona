#include "test_id_mapper.h"

bool test_id_mapper::add_get_id(void *obj, void *target, int *id,
                                crmna_err_t *err) {
  return ((test_id_mapper *)obj)->add_get_id(target, id, err);
}
void *test_id_mapper::find(void *obj, int id, crmna_err_t *err) {
  return ((test_id_mapper *)obj)->find(id, err);
}
bool test_id_mapper::remove(void *obj, int id, crmna_err_t *err) {
  return ((test_id_mapper *)obj)->remove(id, err);
}
bool test_id_mapper::get_iterator(void *obj, id_mapper_iterator_ref *iterator,
                                  crmna_err_t *err) {
  return ((test_id_mapper *)obj)->get_iterator(iterator, err);
}
bool test_id_mapper::free(void *obj, crmna_err_t *err) {
  return ((test_id_mapper *)obj)->free(err);
}

id_mapper test_id_mapper::interface = {.add_get_id = test_id_mapper::add_get_id,
                                       .find = test_id_mapper::find,
                                       .remove = test_id_mapper::remove,
                                       .get_iterator =
                                           test_id_mapper::get_iterator,
                                       .free = test_id_mapper::free};

void test_id_mapper::set_ref(id_mapper_ref *ref) {
  ref->interface = &test_id_mapper::interface;
  ref->obj = this;
}

bool test_id_mapper_factory::create_id_mapper(void *obj, id_mapper_ref *ref,
                                              int min, int max,
                                              crmna_err_t *err) {
  return ((test_id_mapper_factory *)obj)->create_id_mapper(ref, min, max, err);
}

id_mapper_factory test_id_mapper_factory::interface = {
    .create_id_mapper = &test_id_mapper_factory::create_id_mapper};

id_mapper_factory_ref test_id_mapper_factory::get_factory() {
  return {.interface = &test_id_mapper_factory::interface, .obj = this};
}