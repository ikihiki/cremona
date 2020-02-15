#ifndef CREMONA_MESSAGE
#define CREMONA_MESSAGE

#ifdef __KERNEL__
#include <linux/types.h>
#include <net/netlink.h>
#else
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NLMSG_MIN_TYPE 0x10 /* < 0x10: reserved control messages */
#endif



#define INVALID_MESSAGE_TYPE_ID -1

#define CRMNA_GET_STATS (NLMSG_MIN_TYPE * 2)
#define CRMNA_GET_STATS_RESULT ((NLMSG_MIN_TYPE * 3))
typedef struct get_stats_result {
  int total_device_count;
  int total_open_toot;
} get_stats_result_t;
size_t serialize_get_stats_result(const get_stats_result_t *data, char *dest,
                               size_t max_length);

#define CRMNA_CREATE_CONN                                                      \
  (NLMSG_MIN_TYPE + 20) // + 2 is arbitrary. same value for kern/usr

typedef struct create_conn_t {
  char name[50];
} create_conn;

typedef struct create_conn_result_t {
  uint64_t id;
  bool result;
  char *error;
} create_conn_result;

int serialize_create_conn_result(const create_conn_result *data, char *dest,
                                 size_t max_length);
bool deserialize_create_conn(const char *data, size_t length,
                            create_conn *result);

#define CRMNA_DESTROY_DEVICE                                                      \
  (NLMSG_MIN_TYPE + 21) // + 2 is arbitrary. same value for kern/usr

typedef struct destroy_device {
  uint64_t id;
} destroy_device_t;

typedef struct destroy_device_result {
  uint64_t id;
  bool result;
  char *error;
} destroy_device_result_t;

int serialize_destroy_device_result(const destroy_device_result_t *data,
                                    char *dest, size_t max_length);
bool deserialize_destroy_device(const char *data, size_t length,
                                destroy_device_t *result);

#define CRMNA_NEW_TOOT                                                         \
  (NLMSG_MIN_TYPE + 35) // + 2 is arbitrary. same value for kern/usr
#define CRMNA_NEW_TOOT_RESULT                                                  \
  (NLMSG_MIN_TYPE + 45) // + 2 is arbitrary. same value for kern/usr

typedef struct new_toot_t {
  unsigned char uuid[16];
} new_toot;

typedef struct new_toot_result {
  unsigned char uuid[16];
  int result;
} new_toot_result_t;

int serialize_new_toot(const new_toot *data, char *dest, size_t max_length);
int deserialize_new_toot_result(const char *data, size_t length,
                                new_toot_result_t *result);

#define CRMNA_ADD_TOOT_STR (NLMSG_MIN_TYPE + 55)
#define CRMNA_ADD_TOOT_STR_RESULT (NLMSG_MIN_TYPE + 65)

typedef struct add_toot_str {
  unsigned char uuid[16];
  char *text;
} add_toot_str_t;
typedef struct add_toot_str_result {
  unsigned char uuid[16];
  int result;
} add_toot_str_result_t;

int serialize_add_toot_str(const add_toot_str_t *data, char *dest,
                           size_t max_length);
int deserialize_add_toot_str_result(const char *data, size_t length,
                                    add_toot_str_result_t *result);

#define CRMNA_SEND_TOOT (NLMSG_MIN_TYPE + 75)
#define CRMNA_SEND_TOOT_RESULT (NLMSG_MIN_TYPE + 85)
typedef struct send_toot {
  unsigned char uuid[16];
} send_toot_t;
typedef struct send_toot_result {
  unsigned char uuid[16];
  int result;
} send_toot_result_t;

int serialize_send_toot(const send_toot_t *data, char *dest, size_t max_length);
int deserialize_send_toot_result(const char *data, size_t length,
                                 send_toot_result_t *result);

#endif