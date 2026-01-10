#ifndef SERVER_H
#define SERVER_H

#include "game/Game.h"
#include "../shared/socket/Socket.h"
#include <bits/pthreadtypes.h>
#include <stdbool.h>

typedef struct {
  game_t *game;
  pthread_mutex_t *mutex;
  int *commands;
  socket_server_t *socket;
  _Bool running;
} server_t;

typedef struct {
  command_queue_t *commands;
  socket_data_t *soc;
} recver_t;

void server_init(server_t *server, int port, game_setup_t *setup);
void* server_read_commands(void* args);
void server_destroy(server_t *server);

#endif
