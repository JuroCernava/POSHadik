#include "Menu.h"

//Maximalne hodnoty parametrov
#define MIN_WORLD_H  5
#define MAX_WORLD_H  50
#define MIN_WORLD_W  5
#define MAX_WORLD_W  100
#define MIN_PLAYERS  1
#define MAX_PLAYERS  3
#define MAX_LEN_MAIN 9
#define MAX_LEN_WIZ 22
#define MAX_LEN_LOBBY 6
#define MAX_LEN_GAME 10

// Orezavanie hodnoty podla predvoleneho rozsahu
static int clamp_int(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

  static const char *const HELP_TEXT[] = {
  "CONTROLS:",
  "",
  "UP / DOWN   - move in menu",
  "ENTER       - select / change value",
  "",
  "GAME SETUP:",
  "STANDARD    - game ends 10 seconds after the last player leaves",
  "TIMED       - game with time limit",
  "",
  "GAME WORLD OPTIONS:",
  "WORLD HEIGHT    - map height",
  "WORLD WIDTH     - map width",
  "NUMBER OF PLAYERS - players count",
    "OBSTACLES       - enable / disable obstacles",
  };

//MAIN MENU
static const char *const MAIN_MENU_ITEMS[] = {
  "NEW GAME",
  "JOIN GAME",
  "HELP",
  "EXIT"
};

//GAME WIZARD
static const char *const GAME_WIZARD_ITEMS[] = {
  "STANDARD",
  "TIMED",
  "NUMBER OF PLAYERS:",
  "WORLD WIDTH:",
  "WORLD HEIGHT:",
  "OBSTACLES:",
  "START",
  "BACK"
};

//GAME LOBBY
static const char *const GAMES_LOBBY_ITEMS[] = {
  "GAMES:",
  "BACK"
};

// IN-GAME MENU
static const char *const GAME_RUNNING_ITEMS[] = {
  "LEAVE GAME"
};

static void menu_set(menu_t *m, Menu menu, int optionCnt, int longestEntry, int helpShown) {
  m->currMenu = menu;
  m->currOptionCnt = optionCnt;
  m->selectedOptionId = 0;
  m->longestEntry = longestEntry;
  m->helpShown = helpShown;
}

void menu_get_help(const char* const **lines, int *rows) {
  *lines = HELP_TEXT;
  *rows = sizeof(HELP_TEXT) / sizeof(HELP_TEXT[0]);  
}

void menu_init(menu_t *menu) {
  if (!menu) return;
  menu_set(menu, MAIN_MENU, (int)(sizeof(MAIN_MENU_ITEMS) / sizeof(MAIN_MENU_ITEMS[0])), MAX_LEN_MAIN, 0);
  menu->setup.timed = 0;
  menu->setup.obstacles = 0;
  menu->setup.world_h = 15;
  menu->setup.world_w = 25;
  menu->setup.players = 2;
  menu->setup.world_h = clamp_int(menu->setup.world_h, MIN_WORLD_H, MAX_WORLD_H);
  menu->setup.world_w = clamp_int(menu->setup.world_w, MIN_WORLD_W, MAX_WORLD_W);
  menu->setup.players = clamp_int(menu->setup.players, MIN_PLAYERS, MAX_PLAYERS);
}

// konstantne znaky retazcov aj konstantny pointer na adresy 
void get_menu_items(const menu_t *menu, const char *const **items) {
  if (!menu || !items) return;

  switch (menu->currMenu) {
    case MAIN_MENU:     *items = MAIN_MENU_ITEMS;     return;
    case GAME_WIZARD:   *items = GAME_WIZARD_ITEMS;   return;
    case GAMES_LOBBY:   *items = GAMES_LOBBY_ITEMS;   return;
    case GAME_RUNNING:  *items = GAME_RUNNING_ITEMS;  return;
    default:            *items = MAIN_MENU_ITEMS;     return;
  }
}

const game_setup_t* menu_get_setup(const menu_t *menu) {
  return menu ? &menu->setup : 0;
}

void menu_next(menu_t *menu) {
  if (!menu || menu->currOptionCnt <= 0) return;
  menu->selectedOptionId = (menu->selectedOptionId + 1) % menu->currOptionCnt;
}

void menu_prev(menu_t *menu) {
  if (!menu || menu->currOptionCnt <= 0) return;
  menu->selectedOptionId =
      (menu->selectedOptionId - 1 + menu->currOptionCnt) % menu->currOptionCnt;
}

void menu_enter(menu_t *menu) {
  if (!menu) return;
  switch (menu->currMenu) {
    case MAIN_MENU: {
      // 0 NEW GAME, 1 JOIN GAME, 2 HELP, 3 EXIT
      if (menu->selectedOptionId == 0) {
        menu_set(menu, GAME_WIZARD, (int)(sizeof(GAME_WIZARD_ITEMS) / sizeof(GAME_WIZARD_ITEMS[0])), MAX_LEN_WIZ, menu->helpShown);
        return;
      }
      if (menu->selectedOptionId == 1) {
        menu_set(menu, GAMES_LOBBY, (int)(sizeof(GAMES_LOBBY_ITEMS) / sizeof(GAMES_LOBBY_ITEMS[0])), MAX_LEN_LOBBY, menu->helpShown);
        return;
      }
      if (menu->selectedOptionId == 2) {
        menu->helpShown ^= 1;  
        menu_set(menu, MAIN_MENU, (int)(sizeof(MAIN_MENU_ITEMS) / sizeof(MAIN_MENU_ITEMS[0])), MAX_LEN_MAIN, menu->helpShown);
      }
      if (menu->selectedOptionId == 3) {
        menu_set(menu, APP_EXITED, 0, 0, 0);
        return;
      }
      return;
    }
//  "STANDARD",
//  "TIMED",
//  "NUMBER OF PLAYERS:",
//  "WORLD WIDTH:",
//  "WORLD HEIGHT:",
//  "OBSTACLES:",
//  "START",
//  "BACK"

    case GAME_WIZARD: {
      switch (menu->selectedOptionId) {
        case 0:
          menu->setup.timed = 0;
          return;
        case 1: 
          menu->setup.timed = 1;
          menu->timeSetting = !menu->timeSetting; 
          return;
        case 2:
          menu->setup.players++;
          if (menu->setup.players > MAX_PLAYERS) menu->setup.players = MIN_PLAYERS;
          return;
        case 3:
          menu->setup.world_w++;
          if (menu->setup.world_w > MAX_WORLD_W) menu->setup.world_w = MIN_WORLD_W;
          return;
        case 4:
          menu->setup.world_h++;
          if (menu->setup.world_h > MAX_WORLD_H) menu->setup.world_h = MIN_WORLD_H;
          return;
        case 5:
          //bitovy XOR
          menu->setup.obstacles ^= 1;
          return;
        case 6:
          menu_set(menu, GAME_RUNNING, (int)(sizeof(GAME_RUNNING_ITEMS) / sizeof(GAME_RUNNING_ITEMS[0])), MAX_LEN_GAME, menu->helpShown);
          return;
        case 7: 
          menu_set(menu, MAIN_MENU, (int)(sizeof(MAIN_MENU_ITEMS) / sizeof(MAIN_MENU_ITEMS[0])), MAX_LEN_MAIN, menu->helpShown);
          return;
        default:
          return;
        }
    }

    case GAMES_LOBBY:
      /* 0 GAMES:, 1 BACK */
      if (menu->selectedOptionId == 1) {
        menu_set(menu, MAIN_MENU, (int)(sizeof(MAIN_MENU_ITEMS) / sizeof(MAIN_MENU_ITEMS[0])), MAX_LEN_MAIN, menu->helpShown);
      }
      return;

    case GAME_RUNNING:
      /* LEAVE GAME */
      menu_set(menu, MAIN_MENU, (int)(sizeof(MAIN_MENU_ITEMS) / sizeof(MAIN_MENU_ITEMS[0])), MAX_LEN_MAIN, 0);
      return;

    default:
      return;
  }
}
