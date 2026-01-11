#ifndef RENDERER_H
#define RENDERER_H

#include "../menu/Menu.h"
#include "../../shared/position/Position.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

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
    size_t playerCnt;
    size_t *playerLen;      // [playerCnt]
    position_t *pSegments; 
    position_t *foodPos;    // [playerCnt]
    position_t *obstPos;    //#define OBSTACLE_CNT 6
} world_snap_t;

typedef struct {
    uint32_t playerCnt;
    uint32_t segmentCnt;   // súčet playerLen
} world_snap_hdr_t;

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
void render_world_from_snap(const menu_t *menu, const world_snap_t *snap);
void destroy_display(display_t *display);
void snap_destroy(world_snap_t *s);

#endif
