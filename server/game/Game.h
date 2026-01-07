#include <stdbool.h>
#include "../player/Player.h"
#include "../../shared/position/Position.h"
#include <pthread.h>

#ifndef GAME_H
#define GAME_H

typedef enum {
  STANDARD,
  TIMED
} GameMode;

typedef enum {
  FOOD,
  OBSTACLE
} ObjectType;

typedef struct {
  position_t position;
  ObjectType objectType;
} game_object_t;

typedef struct {
  _Bool running;
  size_t playerCap;
  pthread_mutex_t mutex;
  game_object_t *objects;
  player_t *players;
} game_t;

void game_init(game_t *game, int height, int width, int time, _Bool obstacles);
void game_pause(game_t *game, unsigned char playerId);
void add_game_object(game_t *game, position_t* position, ObjectType type);
void update_player_pos(game_t* game, player_t* player);

#endif

