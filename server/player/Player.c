#include "Player.h"
#include <stdlib.h>

Direction opposite(Direction dir) {
    switch(dir) {
        case STATIC: return STATIC;
        case UP: return DOWN;
        case DOWN: return UP;
        case LEFT: return RIGHT;
        case RIGHT: return LEFT;
    }
    return dir;
}

void player_init(player_t *player, unsigned char id, int rows, int cols) {
  player->direction = STATIC;
  player->id = -1;
  player->living = 1;
  player->positions = calloc(5, sizeof(position_t));
  int xPos = (id == 0) ? 5 : rows - 5;
  int yPos = cols / 2.0;
  position_init(&player->positions[0], xPos, yPos);
  player->score = 0;
}

void player_destroy(player_t *player) {
  player->living = 0;
  player->direction = STATIC;
  player->id = -1;
  free(player->positions);
}

void update_player_position(player_t *player, position_t* newPosition) {
  //player->position
}

void update_player_status(player_t *player) {
  player->living = !player->living;
}

void update_player_direction(player_t *player, enum Direction newDirection) {
  if (player->direction == STATIC || newDirection != opposite(player->direction)) {
    player->direction = newDirection;
  }
}
