#include <stdbool.h>
#include <stddef.h>

int add(int a, int b);

typedef struct socket_context socket_context_t;
socket_context_t* create_socket_context();
void destroy_socket_context(socket_context_t *context);
int allocate_id(socket_context_t *context, void *data);
void* remove_id(socket_context_t *context, int id);
void update_id(socket_context_t *context, int id, void(*func)(void *data, void *capture), void *capture);


typedef struct socket{
  socket_context_t *context;
} socket_t;




void init_socket(socket_t *socket);
void destroy_socket(socket_t *socket);
void recive_data_to_socket(socket_t *socket, char *data, size_t size);

typedef void (*reciv_func_t)(int result, int address, int stream, char *data,
                             size_t size, void *context);
void send(socket_t *socket, int address, int stream, char *data, size_t size,
          reciv_func_t reciv_func, void *context);
