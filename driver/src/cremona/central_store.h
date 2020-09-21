#ifndef __CRMNA_CENTRAL_STORE_HEADER__
#define __CRMNA_CENTRAL_STORE_HEADER__

#include "buffer.h"
#include "error.h"
#include "commuinicator.h"

typedef struct store store_t;
typedef struct action action_t;

typedef enum action_type {
  CREATE_DEVICE,
  CREATE_TOOT,
  CREATE_TOOT_RESULT,
  ADD_TOOT_ELEMENT,
  ADD_TOOT_ELEMENT_RESULT,
  SEND_TOOT,
  SEND_TOOT_RESULT,
  DESTROY_DEVICE
} action_type_t;

typedef struct create_device_payload{
    int pid;
    int uid;
    char name[50];
} create_device_payload_t;
bool create_action_from_create_device_message(int pid, crmna_buf_t *message, action_t *action, crmna_err_t *err);
bool create_device(store_t *store, action_t *action, crmna_err_t *err);

typedef struct create_toot_payload {
    int device_id;
} create_toot_payload_t;
bool create_action_create_toot(int device_id, action_t *action, crmna_err_t *err);
bool create_toot(store_t *store, action_t *action, crmna_err_t *err);

typedef struct create_toot_result_payload {
    int pid;
    int device_id;
    int toot_id;
    int result;
} create_toot_result_payload_t;
bool create_action_from_create_toot_result_message(int pid,
                                                   crmna_buf_t *message,
                                                   action_t *action,
                                                   crmna_err_t *err);
bool create_toot_result(store_t *store, action_t *action, crmna_err_t *err);

typedef struct add_toot_element_payload {
  int toot_id;
  crmna_buf_t *text;
} add_toot_element_payload_t;
bool create_action_add_toot_element(int toot_id, crmna_buf_t *txet, action_t *action,
                                    crmna_err_t *err);
bool add_toot_element(store_t *store, action_t *action, crmna_err_t *err);

typedef struct add_toot_element_result_payload {
  int pid;
  int device_id;
  int toot_id;
  int element_id;
  int result;
} add_toot_element_result_payload_t;
bool create_action_from_add_toot_element_result_message(int pid,
                                                   crmna_buf_t *message,
                                                   action_t *action,
                                                   crmna_err_t *err);
bool add_toot_element_result(store_t *store, action_t *action, crmna_err_t *err);

typedef struct send_toot_payload {
  int toot_id;
} send_toot_payload_t;
bool create_action_send_toot(int toot_id, action_t *action, crmna_err_t *err);
bool send_toot(store_t *store, action_t *action, crmna_err_t *err);

typedef struct send_toot_result_payload {
  int pid;
  int device_id;
  int toot_id;
  int result;
} send_toot_result_payload_t;
bool create_action_from_send_toot_result_message(int pid,
                                                   crmna_buf_t *message,
                                                   action_t *action,
                                                   crmna_err_t *err);
bool send_toot_result(store_t *store, action_t *action, crmna_err_t *err);

typedef struct destroy_device_payload {
  int pid;
  int device_id;
} destroy_device_payload_t;
bool create_action_from_destroy_device_message(int pid, crmna_buf_t *message,
                                               action_t *action,
                                               crmna_err_t *err);
bool destroy_device(store_t *store, action_t *action, crmna_err_t *err);

typedef struct action {
    action_type_t type;
    union {
        create_device_payload_t create_device;
        create_toot_payload_t create_toot;
        create_toot_result_payload_t create_toot_result;
        add_toot_element_payload_t add_toot_element;
        add_toot_element_result_payload_t add_toot_element_result;
        send_toot_payload_t send_toot;
        send_toot_result_payload_t send_toot_result;
        destroy_device_payload_t destroy_device;
    } payload;
} action_t;

store_t *create_store(void);
void destroy_store(store_t *store);


bool dispatch(store_t *store, action_t *action, crmna_err_t *err);
bool create_action_from_message(int type, int pid, crmna_buf_t *message,
                                action_t *action, crmna_err_t *err);
bool add_device(store_t *store, int pid, int uid, char *name, int *id, crmna_err_t *err);
void remove_device(store_t *store, int device_id);
bool attach_device_class(store_t *store, int device_id, crmna_err_t *err);
void detach_device_class(store_t *store, int device_id);
void set_device_ready(store_t *store, int device_id);
communicator_ref_t get_communicator(store_t *store);
void set_communicator(communicator_ref_t communicator);

bool add_toot(store_t *store, int device_id, int *toot_id, crmna_err_t *err);
void remove_toot(store_t *store, int toot_id);
bool wait_toot_ready_or_failer(store_t *store, int toot_id, crmna_err_t *err);
bool wait_toot_sent_or_failer(store_t *store, int toot_id, crmna_err_t *err);
void set_toot_failer(store_t *store, int toot_id);
void set_toot_ready(store_t *store, int toot_id);
void set_toot_sent(store_t *store, int toot_id);
int get_device_id_from_toot(store_t *store, int toot_id);
int get_device_pid_from_toot(store_t *store, int toot_id);

bool add_element(store_t *store, int toot_id, int *element_id,
                 crmna_err_t *err);
void remove_element(store_t *store, int element_id);
bool wait_element_sent_or_failer(store_t *store, int element_id,
                                   crmna_err_t *err);
void set_element_sent(store_t *store, int element_id);
void set_element_failer(store_t *store, int element_id);
#endif