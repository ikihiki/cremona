#include "test_logger.h"
#include <cstdarg>
#include <cstdlib>
#include <iostream>

void test_logger::log(cremona_error_level_t level, char *fmt, ...) {
  va_list ap;
  // 可変長引数を１個の変数にまとめる
  va_start(ap, fmt);
  // まとめられた変数で処理する
  std::vprintf(fmt, ap);
  va_end(ap);
  ;
}

logger test_logger::logger_ref = {.log = &test_logger::log};