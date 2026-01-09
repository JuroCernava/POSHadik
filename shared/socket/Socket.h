#ifndef SOCKET_H
#define SOCKET_H

#include <stddef.h>
#include <sys/socket.h>

typedef struct SocketData {
  int socket; //fileDescriptor
} socket_data_t;

typedef struct SocketServer {
  socket_data_t passiveSocket;
  socket_data_t activeSocket;
  int port;
} socket_server_t;

typedef struct SocketClient {
  socket_data_t activeSocket;
  char * serverName;
  int port;
} socket_client_t;

void socket_init(socket_data_t *this, int domain, int type, int protocol);
void socket_destroy(socket_data_t *this);
_Bool socket_is_valid(socket_data_t *this);
void socket_bind(socket_data_t *this, const struct sockaddr *serverAddress, socklen_t severAddressLength);
void socket_listen(socket_data_t * this, int backlog);
void socket_accept(socket_data_t * this, const socket_data_t * passiveSocket, struct sockaddr *clientAddress, socklen_t *clientAddressLength);
_Bool socket_connect(socket_data_t *this, const struct sockaddr *clientAddress, socklen_t clientAddressLength);
void socket_write(socket_data_t *this, const char *buffer, size_t length);
void socket_read(socket_data_t *this, char *buffer, size_t length);


void socket_server_init(socket_server_t *this, int port);
void socket_server_accept_connection(socket_server_t *this);
void socket_server_destroy(socket_server_t *this);

void socket_client_init(socket_client_t *this, char *serverName, char *port);
void socket_client_destroy(socket_client_t *this);
#endif

