#include "test/include/common.h"

int add(int a, int b) { return a + b; }

void init_socket(socket_t *socket) {
  socket->context = create_socket_context();
}

void destroy_socket(socket_t *socket) {
  destroy_socket_context(socket->context);
}

void recive_data_to_socket(socket_t *socket, char *data, size_t size) {}

void send(socket_t *socket, int address, int stream, char *data, size_t size,
          reciv_func_t reciv_func, void *context) {
              
          }


void create_cremona(crmna_t *crmna){

}

void write_data(){

}

