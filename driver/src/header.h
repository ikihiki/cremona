#include <linux/rwsem.h>
#include <linux/wait.h>
#include <net/netlink.h>

#ifndef LINK_HEADER
#define LINK_HEADER

// 作成できるデバイスの上限に達した
#define CRMNA_CREATE_CONN_ERR_NO_SPACE -1
// デバイスが何らかの理由で作成できなかった
#define CRMNA_CREATE_CONN_ERR_FAIL_CREATE_DEVICE -2
// デバイスの割り当てに不整合が発生した。再起動を推薦
#define CRMNA_CREATE_CONN_ERR_FAIL_ALLOC -3

#define CRMNA_RCV_CREATECONN_FAIL_DECODE -4

struct link_data {
  struct sock *socket;
  struct netlink_kernel_cfg config;
};

extern char *crmna_prefix;
extern unsigned int mydevice_major;
extern struct class *mydevice_class;
extern struct link_data link;

int create_link(struct link_data *data, int channel);
int delete_link(struct link_data *data);
int send_message(struct link_data *data, u32 pid, int type, char *msg,
                 int msg_size);

int getNameFromCreateAccountMessage(const char *data, size_t length, char *dest,
                                    size_t max_length);
int writeCreateAccountResultMessage(int result, const char *name, char *dest,
                                    size_t max_length);

int create_connection(const char *name, int pid);
void destroy_all_connections(void);
void init_devices(void);

#define CRMNA_CREATE_CONN                                                      \
  (NLMSG_MIN_TYPE + 25) // + 2 is arbitrary. same value for kern/usr
#define CRMNA_NEW_TOOT                                                         \
  (NLMSG_MIN_TYPE + 35) // + 2 is arbitrary. same value for kern/usr
#define CRMNA_NEW_TOOT_RESULT                                                  \
  (NLMSG_MIN_TYPE + 45) // + 2 is arbitrary. same value for kern/usr

struct new_toot {
  unsigned char uuid[16];
};
typedef struct new_toot_result {
  unsigned char uuid[16];
  int result;
} new_toot_result_t;

int serialize_new_toot(const struct new_toot *data, char *dest,
                       size_t max_length);
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

typedef struct toot_data {
  unsigned char uuid[16];
  int is_open;
  int total_writed_byte;
  int current_writed_byte;
  int current_writed_result;
  int try_write_count;
  int pid;
  char name[10];
  struct toot_data *next;
  struct toot_data *prev;
  wait_queue_head_t open_wait;
  int open_result;
  int try_open_count;
  wait_queue_head_t write_wait;
  wait_queue_head_t close_wait;
  int close_result;
} toot_data_t;

struct toot_data_root {
  struct toot_data *data;
  struct rw_semaphore sem;
};

void init_toot_data_root(struct toot_data_root *list);
void add_toot_data(struct toot_data_root *list, struct toot_data *data);
void delete_toot_data(struct toot_data_root *list, struct toot_data *data);
struct toot_data *serch_toot_data(const struct toot_data_root *list,
                                  const unsigned char uuid[16]);

void notify_open_toot_result(const struct new_toot_result *result);
void notify_add_toot_str_result(const add_toot_str_result_t *result);
void notify_send_toot_result(const send_toot_result_t *result);

#endif