#ifndef CRMNA_LOGGER_HEADER
#define CRMNA_LOGGER_HEADER

#include "../common.h"

#define LOG_IMPL(logger, error, fmt, ...)                                      \
  {                                                                            \
    if (logger != NULL)                                                        \
      logger->log(error, "Cremona: %s: " fmt "\n", __func__, ##__VA_ARGS__);     \
  }

#define LOG_ERROR(logger, fmt, ...) LOG_IMPL(logger, ERROR, fmt, ##__VA_ARGS__)

#define LOG_WARN(logger, fmt, ...) LOG_IMPL(logger, WARN, fmt, ##__VA_ARGS__)

#define LOG_INFO(logger, fmt, ...) LOG_IMPL(logger, INFO, fmt, ##__VA_ARGS__)

#define LOG_AND_ADD_ERROR(logger, crmna_err, fmt, ...)                       \
  {                                                                            \
    LOG_ERROR(logger, fmt, ##__VA_ARGS__);                                     \
    ADD_ERROR(crmna_err, fmt, ##__VA_ARGS__);                                  \
  }

typedef struct {
  void (*log)(cremona_error_level_t level, char *fmt, ...);
} logger;

#endif