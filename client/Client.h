#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>
#include "./menu/Menu.h"
#include "./action manager/ActionManager.h"
#include "./renderer/Renderer.h"
#include <stdbool.h>
#include "../shared/socket/Socket.h"

typedef struct {
  int playerId;
    int gameId;

  action_manager_t actionManager;
  int *messages;
  display_t display;
  menu_t menu;
  } client_t;

typedef struct {
  size_t snapCap;
  size_t inId;
  size_t outId;
  size_t cnt;
  pthread_mutex_t mutex;
  world_snap_t *snaps;
} game_state_t;

typedef struct {
  client_t *client;
  game_state_t gState;
} client_data_t;

typedef struct {
  game_state_t *state;
  socket_data_t *socket;
} recv_data_t;

typedef enum {
  STANDARD,
  TIMED
} GameMode;

void client_init(client_t *client);
void client_listen(client_t *client);
void* client_read_game_snaps(void *args);
void client_destroy(client_t *client);
void client_create_game(client_t *client, int height, int width, GameMode mode, _Bool obstacles);

#endif
