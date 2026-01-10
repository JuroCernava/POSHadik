#include "Server.h"
#include "game/Game.h"

void server_init(server_t *server, int port, game_setup_t* setup) {
  server->running = 1;

  socket_server_init(server->socket, port);
  socket_server_accept_connection(server->socket);
  g_settings_t gSettings;
  g_settings_from_setup(setup, &gSettings);
  game_init(server->game, &gSettings);
}

void server_run_game(server_t *server, world_snap_t *wSnap) {
  game_run(&server->game, wSnap);
}

void server_destroy(server_t *server) {
  socket_server_destroy(server->socket);
  game_destroy(server->game);
  server->running = 0;
}
