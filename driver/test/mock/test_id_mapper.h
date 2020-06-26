#pragma once
#include "interfaces/id_mapper_factory.h"
#include <gmock/gmock.h>

class test_id_mapper {
public:
  virtual ~test_id_mapper() {}
  void set_ref(id_mapper_ref *ref);

  virtual bool add_get_id(void *target, int *id, crmna_err_t *err) = 0;
  virtual void *find(int id, crmna_err_t *err) = 0;
  virtual bool remove(int id, crmna_err_t *err) = 0;
  virtual bool get_iterator(id_mapper_iterator_ref *iterator,
                            crmna_err_t *err) = 0;
  virtual bool free(crmna_err_t *err) = 0;

private:
  static bool add_get_id(void *obj, void *target, int *id, crmna_err_t *err);
  static void *find(void *obj, int id, crmna_err_t *err);
  static bool remove(void *obj, int id, crmna_err_t *err);
  static bool get_iterator(void *obj, id_mapper_iterator_ref *iterator,
                           crmna_err_t *err);
  static bool free(void *obj, crmna_err_t *err);

  static id_mapper interface;
};

class test_id_mapper_mock : public test_id_mapper {
public:
  MOCK_METHOD3(add_get_id, bool(void *target, int *id, crmna_err_t *err));
  MOCK_METHOD2(find, void *(int id, crmna_err_t *err));
  MOCK_METHOD2(remove, bool(int id, crmna_err_t *err));
  MOCK_METHOD2(get_iterator,
               bool(id_mapper_iterator_ref *iterator, crmna_err_t *err));
  MOCK_METHOD1(free, bool(crmna_err_t *err));
};

class test_id_mapper_factory {
public:
  id_mapper_factory_ref get_factory();
  virtual ~test_id_mapper_factory() {}
  virtual bool create_id_mapper(id_mapper_ref *ref, int min, int max,
                                crmna_err_t *err) = 0;

private:
  static bool create_id_mapper(void *obj, id_mapper_ref *ref, int min, int max,
                               crmna_err_t *err);

  static id_mapper_factory interface;
};

class test_id_mapper_factory_mock : public test_id_mapper_factory {
public:
  MOCK_METHOD4(create_id_mapper,
               bool(id_mapper_ref *ref, int min, int max, crmna_err_t *err));
};
