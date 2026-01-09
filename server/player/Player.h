#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "../../shared/position/Position.h"

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  STATIC
} Direction;

typedef struct {
  int id;
  position_t *positions;
  Direction direction;
  _Bool living;
  _Bool running;
  int score;
} player_t;


void player_init(player_t *player, unsigned char id, int startX, int startY);
Direction opposite(Direction dir);
void update_player_positions(player_t* player, int rows, int cols);
void update_player_status(player_t *player);
void update_player_direction(player_t *player, Direction newDirection);
void player_grow(player_t *player);
void player_pause(player_t *player);
void player_destroy(player_t *player);

#endif
