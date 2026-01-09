#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>
#include "./menu/Menu.h"
#include "./action manager/ActionManager.h"
#include "./renderer/Renderer.h"
#include <stdbool.h>

typedef struct {
  int playerId;
      int gameId;

  action_manager_t actionManager;
  pthread_mutex_t mutex;

  int *messages;
  display_t display;
  menu_t menu;
} client_t;

typedef struct {
  int height;
  int width;
  size_t obstacleCnt;
  size_t playerCnt;
  int* playerLens;
  position_t *foodPos;
  position_t *obstaclePos;
  position_t *pSegments;
} world_snap_t;

void client_init(client_t *client);
void client_listen(client_t *client);
void client_destroy(client_t *client);
void client_create_game(client_t *client, int height, int width, GameMode mode, _Bool obstacles);

#endif
