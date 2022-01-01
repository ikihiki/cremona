#include <stdbool.h>
#include <stddef.h>

int add(int a, int b);

typedef struct socket_context socket_context_t;
socket_context_t* create_socket_context();
void destroy_socket_context(socket_context_t *context);
int allocate_id(socket_context_t *context, void *data);
void* remove_id(socket_context_t *context, int id);
void update_id(socket_context_t *context, int id, void(*func)(void *data, void *capture), void *capture);

typedef struct {}dic_t;




typedef struct {
  char name[100];
  int pid;
} device_context_t;


typedef struct packet {
  char *data;
  uint32_t offset;
  uint32_t length;
}

typedef struct {
  uint32_t stream_id;
  dic_t *send_buffer
  dic_t *recive_buffer;
} stream_t


typedef struct {
  dic_t *streams;
  dic_t *pending_packets;
} connection_t

typedef struct socket{
  socket_context_t *context;
  dic_t *connections;
} socket_t;




void init_socket(socket_t *socket);
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
  dic_t *devices;
} crmna_t;
