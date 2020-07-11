#pragma once
#include "cremona.h"
#include "test_communicator.h"
#include "test_id_mapper.h"
#include "test_locker.h"
#include "test_waiter.h"

class test_communicate
{

public:
  test_communicate(logger *logger_ref);
  test_communicate(cremona_device_manager *device_manager,
                   logger *logger_ref);

  ~test_communicate();
  communicate_t *get_communicate();

  test_locker_factory_mock locker_factory_mock;
  test_waiter_factory_mock waiter_factory_mock;
  test_communicator_factory_mock communicator_factory;
  test_id_mapper_factory_mock id_mapper_factory;
  communicate_t communicate;  
};
