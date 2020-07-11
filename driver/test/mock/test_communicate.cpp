#include "test_communicate.h"
#include "cremona_internal.h"

test_communicate::test_communicate(logger *logger_ref) {
  init_communicate(&this->communicate, communicator_factory.get_mock_factory(),
                   locker_factory_mock.get_mock_factory(),
                   id_mapper_factory.get_mock_factory(), NULL, logger_ref,
                   NULL);
}

test_communicate::test_communicate(cremona_device_manager *device_manager,
                                   logger *logger_ref) {
  init_communicate(&this->communicate, communicator_factory.get_mock_factory(),
                   locker_factory_mock.get_mock_factory(),
                   id_mapper_factory.get_mock_factory(), device_manager,
                   logger_ref, NULL);
}

test_communicate::~test_communicate() {
  destory_communicate(&this->communicate);
}

communicate_t *test_communicate::get_communicate() {
  return &this->communicate;
}