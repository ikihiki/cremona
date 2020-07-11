#pragma once
#include "interfaces/device_file_factory.h"
#include <gmock/gmock.h>

class test_device_file {
public:
  virtual ~test_device_file(){};
  void set_ref(device_file_ref *ref);
  virtual void free() = 0;

private:
  static void free(void *obj);

  static device_file interface;
};

class test_device_file_mock : public test_device_file {
public:
  MOCK_METHOD0(free, void());
};

class test_device_file_factory {
public:
  virtual ~test_device_file_factory() {}
  device_file_factory_ref get_factory();
  virtual bool create_device_file(cremona_device_t *device,
                                  device_file_ref *device_file,
                                  crmna_err *err) = 0;

private:
  static bool create_device_file(void *obj, cremona_device_t *device,
                                 device_file_ref *device_file, crmna_err *err);

  static device_file_factory interface;
};

class test_device_file_factory_mock : public test_device_file_factory {
public:
  test_device_file_factory_mock();
  MOCK_METHOD3(create_device_file,
               bool(cremona_device_t *device, device_file_ref *device_file,
                    crmna_err *err));

  test_device_file_mock mocks[10];
  int next_mock = 0;
  device_file_factory_ref ref;
  device_file_factory_ref *get_mock_factory();
};