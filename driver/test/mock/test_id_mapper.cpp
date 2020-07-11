#include "test_id_mapper.h"
using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

bool test_id_mapper::add_get_id(void *obj, void *target, int *id,
                                crmna_err *err) {
  return ((test_id_mapper *)obj)->add_get_id(target, id, err);
}
void *test_id_mapper::find(void *obj, int id, crmna_err *err) {
  return ((test_id_mapper *)obj)->find(id, err);
}

bool test_id_mapper::replace(void *obj, int id, void *new_one,
                             void **replaced_entry, crmna_err *err) {
  return ((test_id_mapper *)obj)->replace(id, new_one, replaced_entry, err);
}

bool test_id_mapper::remove(void *obj, int id, void **removed_entry,
                            crmna_err *err) {
  return ((test_id_mapper *)obj)->remove(id, removed_entry, err);
}
bool test_id_mapper::get_iterator(void *obj, id_mapper_iterator_ref *iterator,
                                  crmna_err *err) {
  return ((test_id_mapper *)obj)->get_iterator(iterator, err);
}
void test_id_mapper::free(void *obj) { ((test_id_mapper *)obj)->free(); }

id_mapper test_id_mapper::interface = {.add_get_id = test_id_mapper::add_get_id,
                                       .find = test_id_mapper::find,
                                       .replace = test_id_mapper::replace,
                                       .remove = test_id_mapper::remove,
                                       .get_iterator =
                                           test_id_mapper::get_iterator,
                                       .free = test_id_mapper::free};

void test_id_mapper::set_ref(id_mapper_ref *ref) {
  ref->interface = &test_id_mapper::interface;
  ref->obj = this;
}

test_id_mapper_mock::test_id_mapper_mock() {
  ON_CALL(*this, free()).WillByDefault(Return());
  ON_CALL(*this, add_get_id(_, _, _))
      .WillByDefault(Invoke([this](void *target, int *id, crmna_err *) {
        this->map.insert(std::make_pair(this->next_id, target));
        *id = this->next_id;
        this->next_id++;
        return true;
      }));

  ON_CALL(*this, find(_, _))
      .WillByDefault(Invoke([this](int id, crmna_err *) -> void * {
        if (this->map.find(id) == this->map.end()) {
          return NULL;
        } else {
          return this->map.at(id);
        }
      }));

  ON_CALL(*this, replace(_, _, _, _))
      .WillByDefault(Invoke(
          [this](int id, void *new_one, void **replaced_entry, crmna_err *) {
            auto entry = this->map.find(id);
            if (entry == this->map.end()) {
              return false;
            }
            if (replaced_entry != NULL) {
              *replaced_entry = entry->second;
            }
            this->map[id] = new_one;
            return true;
          }));
  ON_CALL(*this, remove(_, _, _))
      .WillByDefault(Invoke([this](int id, void **removed_entry, crmna_err *) {
        if (removed_entry != NULL) {
          *removed_entry = this->map.at(id);
        }
        this->map.erase(id);
        return true;
      }));
}

bool test_id_mapper_factory::create_id_mapper(void *obj, id_mapper_ref *ref,
                                              int min, int max,
                                              crmna_err *err) {
  return ((test_id_mapper_factory *)obj)->create_id_mapper(ref, min, max, err);
}

id_mapper_factory test_id_mapper_factory::interface = {
    .create_id_mapper = &test_id_mapper_factory::create_id_mapper};

id_mapper_factory_ref test_id_mapper_factory::get_factory() {
  return {.interface = &test_id_mapper_factory::interface, .obj = this};
}

test_id_mapper_factory_mock::test_id_mapper_factory_mock() {
  this->ref = this->get_factory();

  ON_CALL(*this, create_id_mapper(_, _, _, _))
      .WillByDefault(Invoke([this](id_mapper_ref *ref, int, int, crmna_err *) {
        if (this->next_mock == 10) {
          throw std::runtime_error("overflow mock");
        }
        this->mocks[this->next_mock].set_ref(ref);
        this->next_mock++;
        return true;
      }));
}

id_mapper_factory_ref *test_id_mapper_factory_mock::get_mock_factory(){
  return &this->ref;
}