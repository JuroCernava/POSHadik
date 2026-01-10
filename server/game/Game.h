#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdatomic.h>
#include "../player/Player.h"
#include "../../shared/position/Position.h"
#include <pthread.h>
#include "../../shared/socket/Socket.h"
typedef enum {
  STANDARD,
  TIMED
} GameMode;

typedef enum {
  FOOD,
  OBSTACLE
} ObjectType;

typedef struct {
  int startX;
  int startY;
  int endX;
  int endY;
} world_corner_t;

typedef struct {
  world_corner_t *corners;
  GameMode mode;
  int height;
  int width;
  int time;
  _Bool obstacles;
} g_settings_t;

typedef struct {
  position_t position;
  ObjectType objectType;
} game_object_t;

typedef struct {
  game_object_t *objects;
  player_t *players;
  g_settings_t settings;
  size_t playerCap;
  size_t playerCnt;
  size_t objectsCap;
  size_t objectsCnt;
  _Bool running;
} game_t;

typedef struct {
  size_t obstacleCnt;
  size_t playerCnt;
  int* playerScores;
  position_t *foodPos;
  position_t *obstaclePos;
  position_t *pSegments;
} world_snap_t;

typedef struct {
  int timed;        /* 0=STANDARD, 1=TIMED */
  int obstacles;    /* 0/1 */
  double time;
  int world_h;
  int world_w;
  int players;
} game_setup_t;

typedef struct {
  int *commands;
  size_t commandCnt, inId, outId, cap;
  //pthread_cond_t queueFull;
  pthread_mutex_t mutex; 
} command_queue_t;

typedef struct {
  game_t *game;
  world_snap_t *snap;
  command_queue_t commands;
  socket_data_t *activeSocket; 
} game_args_t;

void game_init(game_t *game, g_settings_t *settings);
void game_pause(game_t *game, unsigned char playerId);

void game_to_snap(game_t *game, world_snap_t *snap);
void* game_run(void *args);
//void game_run(game_t *game, world_snap_t *snap, _Bool *snapRdy, socket_data_t *activeSocket);
void game_update_p_direction(game_t *game, int pId, Direction newDir);
void update_player_pos(game_t* game, player_t* player);
void g_settings_from_setup(const game_setup_t *s, g_settings_t *settings);
void game_destroy(game_t *game);
void snap_destroy(world_snap_t *s);

#endif

