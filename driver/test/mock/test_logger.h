#pragma once
#include "interfaces/logger.h"

class test_logger {
public:
  static logger logger_ref;

private:
  static void log(cremona_error_level_t level, char *fmt, ...);
};