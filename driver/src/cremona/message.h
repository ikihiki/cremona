#ifndef CREMONA_MESSAGE
#define CREMONA_MESSAGE
#include "buffer.h"
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

#define CRMNA_GET_HEALTH (NLMSG_MIN_TYPE * 2)
#define CRMNA_GET_HEALTH_RESULT (NLMSG_MIN_TYPE * 3)
typedef struct get_health {
  uint32_t device_id;
} get_health_t;
typedef struct get_health_result {
  uint32_t device_id;
  bool status;
} get_health_result_t;
bool serialize_get_health_result(const get_health_result_t *data,
                                 crmna_buf_t *dest);
bool deserialize_get_health(const crmna_buf_t *data, get_health_t *result);

#define CRMNA_CREATE_DEVICE (NLMSG_MIN_TYPE * 4)
#define CRMNA_CREATE_DEVICE_RESULT (NLMSG_MIN_TYPE * 5)
typedef struct create_device {
  char name[50];
  uint32_t uid;
} create_device_t;
typedef struct create_device_result {
  uint32_t id;
} create_device_result_t;
bool serialize_create_device_result(const create_device_result_t *data,
                                    crmna_buf_t *dest);
bool deserialize_create_conn(const crmna_buf_t *data, create_device_t *result);

#define CRMNA_DESTROY_DEVICE                                                   \
  (NLMSG_MIN_TYPE * 6) // + 2 is arbitrary. same value for kern/usr
#define CRMNA_DESTROY_DEVICE_RESULT                                            \
  (NLMSG_MIN_TYPE * 7) // + 2 is arbitrary. same value for kern/usr
typedef struct destroy_device {
  uint32_t id;
} destroy_device_t;
typedef struct destroy_device_result {
  uint32_t id;
} destroy_device_result_t;
bool serialize_destroy_device_result(const destroy_device_result_t *data,
                                     crmna_buf_t *dest);
bool deserialize_destroy_device(const crmna_buf_t *data,
                                destroy_device_t *result);

#define CRMNA_NEW_TOOT                                                         \
  (NLMSG_MIN_TYPE * 8) // + 2 is arbitrary. same value for kern/usr
#define CRMNA_NEW_TOOT_RESULT                                                  \
  (NLMSG_MIN_TYPE * 9) // + 2 is arbitrary. same value for kern/usr

typedef struct new_toot {
  uint32_t toot_id;
  uint32_t device_id;
} new_toot_t;

typedef struct new_toot_result {
  uint32_t toot_id;
  uint32_t device_id;
  int result;
} new_toot_result_t;

bool serialize_new_toot(const new_toot_t *data, crmna_buf_t *dest);
bool deserialize_new_toot_result(const crmna_buf_t *data,
                                 new_toot_result_t *result);

#define CRMNA_ADD_TOOT_TEXT (NLMSG_MIN_TYPE * 10)
#define CRMNA_ADD_TOOT_TEXT_RESULT (NLMSG_MIN_TYPE * 11)

typedef struct add_toot_element {
  uint32_t toot_id;
  uint32_t device_id;
  uint32_t element_id;
  uint32_t index;
  char *text;
} add_toot_text_t;
typedef struct add_toot_element_result {
  uint32_t toot_id;
  uint32_t device_id;
  uint32_t element_id;
  int result;
} add_toot_text_result_t;

bool serialize_add_toot_element(const add_toot_text_t *data, crmna_buf_t *dest);
bool deserialize_add_toot_element_result(const crmna_buf_t *data,
                                         add_toot_text_result_t *result);

#define CRMNA_SEND_TOOT (NLMSG_MIN_TYPE * 12)
#define CRMNA_SEND_TOOT_RESULT (NLMSG_MIN_TYPE * 13)
typedef struct send_toot {
  uint32_t toot_id;
  uint32_t device_id;
} send_toot_t;
typedef struct send_toot_result {
  uint32_t toot_id;
  uint32_t device_id;
  int result;
} send_toot_result_t;

bool serialize_send_toot(const send_toot_t *data, crmna_buf_t *dest);
bool deserialize_send_toot_result(const crmna_buf_t *data,
                                  send_toot_result_t *result);

#endif