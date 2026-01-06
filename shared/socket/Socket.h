#ifndef SOCKET_H
#define SOCKET_H
#include <stddef.h>
#include <sys/socket.h>

typedef struct {
  char** buffer;
  int fileDescriptor;
  _Bool isConnected;
  struct sockaddr socAddress;
} socket_t;

typedef struct {
  socket_t activeSocket;
  char* serverName;
} client_socket_t;

typedef struct {
socket_t passiveSocket;
socket_t activeSocket;
int port;
} socket_server_t;


void socket_init(socket_t * this, int domain, int type, int protocol);
void socket_destroy(socket_t * this);
_Bool socket_is_valid(socket_t * this);
void socket_bind(socket_t * this, const struct sockaddr * severAddress, socklen_t
severAddressLength);
void socket_listen(socket_t * this, int backlog);
void socket_accept(socket_t * this, const socket_t * passiveSocket, struct sockaddr
* clientAddress, socklen_t * clientAddressLength);
_Bool socket_connect(socket_t * this, const struct sockaddr * clientAddress, socklen_t
clientAddressLength);
void socket_write(socket_t * this, const char * buffer, size_t length);
void socket_read(socket_t * this, char * buffer, size_t length);


void socket_server_init(socket_server_t * this, int port);
void socket_server_accept_connection(socket_server_t * this);
void socket_server_destroy(socket_server_t * this);
// Štruktúra obsahujúca informácie pre prácu klienta

// Hlavičky funkcií, ktoré sú verejne dostupné a pracujú s informáciami pre klienta
void socket_client_init(client_socket_t *this, char * serverName, char * port);
void socket_client_destroy(client_socket_t *this);
#endif

