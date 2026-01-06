#include "../../server/game/Game.h"
#include "../menu/Menu.h"
#ifndef RENDERER_H
#define RENDERER_H

enum Window {
  MENU,
  GAME
};

typedef struct {
  enum Window window;
  game_t *currGame;
} display_t;

void init_display(display_t *display);
void render_frame(int rows, int cols);
void render_interface(menu_t *menu);
void render_message(char* message);
void render_help(menu_t *menu);
void destroy_display(display_t *display);


#endif
