#ifndef RENDERER_H
#define RENDERER_H

#include "../menu/Menu.h"
#include "../../shared/position/Position.h"
#include <stdlib.h>

enum Window {
  MENU,
  GAME
};

typedef struct {
  int startX;
  int startY;
  int endX;
  int endY;
} world_corner_t;

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

typedef struct {
  enum Window window;
} display_t;

void init_display(display_t *display);
void render_frame(int rows, int cols, world_corner_t *corns);
void render_interface(menu_t *menu);
void render_message(char* message);
void render_help(menu_t *menu);
void render_game_world(menu_t *menu);
void render_player(position_t *newStart, position_t *oldEnd, _Bool init);
void destroy_display(display_t *display);


#endif
