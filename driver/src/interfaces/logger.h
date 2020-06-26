#ifndef CRMNA_LOGGER_HEADER
#define CRMNA_LOGGER_HEADER

#include "../common.h"

#define LOG_IMPL(logger, error, fmt, ...)                                      \
  (logger->log(error, "Cremona: %s: " fmt "\n", __func__, __VA_ARGS__))

#define LOG_ERROR(device_manager, fmt, ...)                                    \
  LOG_IMPL(device_manager, ERROR, fmt, __VA_ARGS__)

#define LOG_WARN(device_manager, fmt, ...)                                     \
  LOG_IMPL(device_manager, WARN, fmt, __VA_ARGS__)

#define LOG_INFO(device_manager, fmt, ...)                                     \
  LOG_IMPL(device_manager, INFO, fmt, __VA_ARGS__)

#define LOG_AND_WRITE_ERROR(device_manager, crmna_err, fmt, ...)               \
  {                                                                            \
    LOG_ERROR(device_manager, fmt, __VA_ARGS__);                               \
    snprintf(crmna_err->error_msg, crmna_err->error_msg_len, fmt,              \
             __VA_ARGS__);                                                     \
  }

typedef struct {
  void (*log)(cremona_error_level_t level, char *fmt, ...);
} logger;

#endif