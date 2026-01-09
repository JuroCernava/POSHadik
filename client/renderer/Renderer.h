#ifndef RENDERER_H
#define RENDERER_H

#include "../../server/game/Game.h"
#include "../menu/Menu.h"
#include "../../shared/position/Position.h"

enum Window {
  MENU,
  GAME
};

typedef struct {
  enum Window window;
  game_t *currGame;
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
