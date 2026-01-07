#include <stdbool.h>
#include "../../shared/position/Position.h"

#ifndef PLAYER_H
#define PLAYER_H

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  STATIC
} Direction;

typedef struct {
  unsigned char id;
  position_t *positions;
  Direction direction;
  _Bool living;
  int score;
} player_t;

void player_init(player_t *player, unsigned char id, int rows, int cols);
void update_player_position(player_t *player, position_t* newPosition);
void update_player_status(player_t *player);
void update_player_direction(player_t *player, Direction newDirection);
void player_destroy(player_t *player);

#endif
