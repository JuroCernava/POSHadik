#ifndef SERVER_H
#define SERVER_H

#include "game/Game.h"
#include "../shared/socket/Socket.h"
#include <stdbool.h>



typedef struct {
  game_t *game;
  socket_server_t *socket;
  _Bool running;
} server_t;

void server_init(server_t *server, game_t *newGame);
void server_listen(server_t *server);
void server_accept_conn(server_t *server);
void server_destroy(server_t *server);

#endif
