#ifndef MENU_H
#define MENU_H

typedef enum {
  MAIN_MENU = 0,
  GAME_WIZARD,     
  GAMES_LOBBY,
  GAME_RUNNING,
  APP_EXITED
} Menu;

typedef struct {
  int timed;        /* 0=STANDARD, 1=TIMED */
  int obstacles;    /* 0/1 */
  int world_h;
  int world_w;
  int players;
} game_setup_t;

typedef struct {
  Menu currMenu;
  int selectedOptionId;
  int currOptionCnt;
  int longestEntry;
  int helpShown;
  game_setup_t setup;
} menu_t;

void menu_init(menu_t *menu);
void menu_next(menu_t *menu);
void menu_prev(menu_t *menu);
void menu_enter(menu_t *menu);
void menu_get_help(const char* const **lines, int *rows);
void get_menu_items(const menu_t *menu, const char *const **items);

const game_setup_t* menu_get_setup(const menu_t *menu);

#endif
