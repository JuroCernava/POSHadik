#include "Player.h"
#include <stdlib.h>
#include <stdio.h>

#define ARR_EXP_CONST 5 

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

static void direction_to_position(position_t *pos, Direction dir) {
  switch(dir) {
    case STATIC:
      break;
    case UP: 
      pos->yPos--;
      break;
    case DOWN:
      pos->yPos++;
      break;
    case LEFT:
      pos->xPos--;
      break;
    case RIGHT:
      pos->xPos++;
      break;
  }
}

void player_init(player_t *player, unsigned char id, int startX, int startY) {
  player->direction = STATIC;
  player->id = id;
  player->living = 1;
  player->positions = calloc(ARR_EXP_CONST, sizeof(position_t));
  position_init(&player->positions[0], startX, startY);
  player->score = 0;
  player->running = 1;
}

void player_destroy(player_t *player) {
  player->living = 0;
  player->direction = STATIC;
  player->id = -1;
  player->running = 0;
  free(player->positions);
}

void player_grow(player_t *player) {
  if (++player->score % ARR_EXP_CONST > 0) {
    position_init(&player->positions[player->score], player->positions[player->score - 1].xPos,
                  player->positions[player->score - 1].yPos);
  } else {
    position_t *tmpPositions = realloc(player->positions, ( player->score  + ARR_EXP_CONST) * sizeof(position_t));
    if (tmpPositions == NULL) {
      perror("realloc (positions)!");
      player->score--;
      return;
    } else {
      player->positions = tmpPositions;
      position_init(&player->positions[player->score], player->positions[player->score - 1].xPos,
                  player->positions[player->score - 1].yPos); 
    }
  } 
}

void update_player_positions(player_t* player, int rows, int cols) {
  int length = player->score + 1;
  for (int pid = length - 1; pid >= 0; --pid) {
    if  (pid == 0) {
      direction_to_position(&player->positions[0], player->direction);
    } else {
      position_init(&player->positions[pid], player->positions[pid - 1].xPos, player->positions[pid - 1].yPos);
    }
    if (player->positions[pid].yPos < 0) {
      player->positions[pid].yPos = rows - 1;
    } else if (player->positions[pid].yPos >= rows) {
      player->positions[pid].yPos = 0;
    }
    if (player->positions[pid].xPos < 0) {
      player->positions[pid].xPos = cols - 1;
    } else if (player->positions[pid].xPos >= cols) {
      player->positions[pid].xPos = 0;
    }
  }
}

void player_pause(player_t *player) {
  player->running = 0;
}

void update_player_status(player_t *player) {
  player->direction = STATIC;
  player->living = 0;
}

void update_player_direction(player_t *player, Direction newDirection) {
  if (player->direction == STATIC || newDirection != opposite(player->direction)) {
    player->direction = newDirection;
  }
}
