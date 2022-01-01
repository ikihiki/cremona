#include "test/include/common.h"

int add(int a, int b) { return a + b; }

void init_socket(socket_t *socket, socket_context_t *context) {
  socket->context = context;
  socket->connections = create_id_map();
}

void destroy_socket(socket_t *socket) {
  destroy_id_map(socket->connections);
}

void recive_data_to_socket(socket_t *socket, char *data, size_t size) {}

void send(socket_t *socket, int address, int stream, char *data, size_t size,
          reciv_func_t reciv_func, void *context) {
              
          }


void create_cremona(crmna_t *crmna, socket_t *socket){
  crmna->socket = socket;
  crmna->devices = create_id_map();
}

void write_data(){

}

