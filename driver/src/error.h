#ifndef CRMNA_ERROR_HEADER
#define CRMNA_ERROR_HEADER

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/types.h>
#else
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#endif

typedef enum cremona_error_level { ERROR, WARN, INFO } cremona_error_level_t;

#define ERROR_MESSAGE_ENTRY_LENGTH 100
#define ERROR_MESSAGE_MAX_ENTRY 10

typedef struct {
  char error_msg[ERROR_MESSAGE_MAX_ENTRY][ERROR_MESSAGE_ENTRY_LENGTH];
  int curret_line;
} crmna_err;

#define DEFINE_ERROR(name) crmna_err name = {.curret_line = 0}

#define ADD_ERROR(crmna_err, fmt, ...)                                         \
  {                                                                            \
    if (crmna_err != NULL &&                                                   \
        crmna_err->curret_line < ERROR_MESSAGE_MAX_ENTRY) {                    \
      snprintf(crmna_err->error_msg[crmna_err->curret_line],                   \
               ERROR_MESSAGE_ENTRY_LENGTH, "%s: " fmt, __func__,               \
               ##__VA_ARGS__);                                                 \
      crmna_err->curret_line++;                                                \
    }                                                                          \
  }

#define HAS_ERROR(crmna_err)                                                   \
  ((crmna_err) != NULL && (crmna_err)->curret_line > 0)

#define CAT_I(x, y) x##y
#define CAT(x, y) CAT_I(x, y)

#define INC0 1
#define INC1 2
#define INC2 3
#define INC3 4
#define INC4 5
#define INC5 6
#define INC6 7
#define INC7 8
#define INC8 9
#define INC(i) CAT(INC, i)

#define EMPTY(...)
#define DEF_COMMA0 _, 1 EMPTY
#define COMMA0() , 0
#define IS_EMPTY_III(f, s) s
#define IS_EMPTY_II(t) IS_EMPTY_III t
#define IS_EMPTY_I(x) IS_EMPTY_II((DEF_##x()))
#define IS_EMPTY(key, ...) IS_EMPTY_I(key COMMA0)

#define IF_0(x, y) y
#define IF_1(x, y) x
#define IF(cond, x, y) CAT(IF_, cond)(x, y)

#define FOR_EACH_I9(i, F, key, value, ...)                                     \
  F(key, value)                                                                \
  IF(IS_EMPTY(__VA_ARGS__), EMPTY, ", " CAT(FOR_EACH_I, i))                    \
  (INC(i), F, __VA_ARGS__)
#define FOR_EACH_I8(i, F, key, value, ...)                                     \
  F(key, value)                                                                \
  IF(IS_EMPTY(__VA_ARGS__), EMPTY, ", " CAT(FOR_EACH_I, i))                    \
  (INC(i), F, __VA_ARGS__)
#define FOR_EACH_I7(i, F, key, value, ...)                                     \
  F(key, value)                                                                \
  IF(IS_EMPTY(__VA_ARGS__), EMPTY, ", " CAT(FOR_EACH_I, i))                    \
  (INC(i), F, __VA_ARGS__)
#define FOR_EACH_I6(i, F, key, value, ...)                                     \
  F(key, value)                                                                \
  IF(IS_EMPTY(__VA_ARGS__), EMPTY, ", " CAT(FOR_EACH_I, i))                    \
  (INC(i), F, __VA_ARGS__)
#define FOR_EACH_I5(i, F, key, value, ...)                                     \
  F(key, value)                                                                \
  IF(IS_EMPTY(__VA_ARGS__), EMPTY, ", " CAT(FOR_EACH_I, i))                    \
  (INC(i), F, __VA_ARGS__)
#define FOR_EACH_I4(i, F, key, value, ...)                                     \
  F(key, value)                                                                \
  IF(IS_EMPTY(__VA_ARGS__), EMPTY, ", " CAT(FOR_EACH_I, i))                    \
  (INC(i), F, __VA_ARGS__)
#define FOR_EACH_I3(i, F, key, value, ...)                                     \
  F(key, value)                                                                \
  IF(IS_EMPTY(__VA_ARGS__), EMPTY, ", " CAT(FOR_EACH_I, i))                    \
  (INC(i), F, __VA_ARGS__)
#define FOR_EACH_I2(i, F, key, value, ...)                                     \
  F(key, value)                                                                \
  IF(IS_EMPTY(__VA_ARGS__), EMPTY, ", " CAT(FOR_EACH_I, i))                    \
  (INC(i), F, __VA_ARGS__)
#define FOR_EACH_I1(i, F, key, value, ...)                                     \
  F(key, value)                                                                \
  IF(IS_EMPTY(__VA_ARGS__), EMPTY, ", " CAT(FOR_EACH_I, i))                    \
  (INC(i), F, __VA_ARGS__)
#define FOR_EACH_I0(i, F, key, value, ...)                                     \
  F(key, value)                                                                \
  IF(IS_EMPTY(__VA_ARGS__), EMPTY, ", " CAT(FOR_EACH_I, i))                    \
  (INC(i), F, __VA_ARGS__)
#define FOR_EACH_I(i, F, ...)                                                  \
  IF(IS_EMPTY(__VA_ARGS__), EMPTY, CAT(FOR_EACH_I, i))(INC(i), F, __VA_ARGS__)
#define FOR_EACH(F, ...) FOR_EACH_I(0, F, __VA_ARGS__)
#define JSON_ELEMENT(key, value) "\"" #key "\": \"%" #value "\""
#define STRING(str) #str
#define JSON_OUT(...) "{" FOR_EACH(JSON_ELEMENT, __VA_ARGS__) "}"

#endif