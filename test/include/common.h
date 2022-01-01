#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
int add(int a, int b);

typedef struct id_map id_map_t;
id_map_t* create_id_map();
void destroy_id_map(id_map_t *map);
int allocate_id(id_map_t *map, void *data);
void add_id(id_map_t *map, int id, void *data);
void* remove_id(id_map_t *map, int id);
size_t get_length(id_map_t *map);
void update_id(id_map_t *map, int id, void(*func)(void *data, void *context), void *context);
void for_each_id_map(id_map_t *map, void(*func)(int id, void *data, void *context), void *context);


typedef struct socket_context socket_context_t;

typedef struct {
  char name[100];
  int pid;
} device_context_t;


typedef struct packet {
  char *data;
  uint32_t offset;
  uint32_t length;
} packet_t;

typedef struct {
  uint32_t stream_id;
  id_map_t *send_buffer;
  id_map_t *recive_buffer;
} stream_t;


typedef struct {
  id_map_t *streams;
  id_map_t *pending_packets;
} connection_t;

typedef struct socket{
  socket_context_t *context;
  id_map_t *connections;
} socket_t;




void init_socket(socket_t *socket, socket_context_t *context);
void destroy_socket(socket_t *socket);
void recive_data_to_socket(socket_t *socket, char *data, size_t size);

void create_packet();

void create_stream();
void append_stream();
void comlete_stream();
void wait_stream();


typedef void (*reciv_func_t)(int result, int address, int stream, char *data,
                             size_t size, void *context);
void send(socket_t *socket, int address, int stream, char *data, size_t size,
          reciv_func_t reciv_func, void *context);

typedef struct {
  id_map_t *devices;
  socket_t *socket;
} crmna_t;

void create_cremona(crmna_t *crmna, socket_t *socket);

bool dump_state(crmna_t *crmna, void *buff, size_t size);
