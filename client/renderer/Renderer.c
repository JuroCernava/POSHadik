#include "Renderer.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
//enum Window {
//  MENU,
//  GAME
//};
//
//typedef struct {
//  enum Window window;
//  game_t *currGame;
//} display_t;

void init_display(display_t *display) {
  display->window = MENU;
  display->currGame = NULL;
  initscr();
  noecho();
  curs_set(0);
}

static void align_text(const char * const *lines, int count, int skip,
                       int *xStart, int *yStart, int *longestOut) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    if (!lines || count <= 0 || skip < 0 || skip >= count) {
        if (xStart) *xStart = 0;
        if (yStart) *yStart = 0;
        if (longestOut) *longestOut = 0;
        return;
    }

    int longest = 0;
    for (int i = skip; i < count; ++i) {
        int len = (int)strlen(lines[i]);
        if (len > longest) longest = len;
    }

    int visible = count - skip;

    int x0 = (cols - longest) / 2;
    int y0 = (rows - visible) / 2;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;

    if (xStart) *xStart = x0;
    if (yStart) *yStart = y0;
    if (longestOut) *longestOut = longest;
}

void render_frame(int rows, int cols) {
  int maxX,maxY;
  getmaxyx(stdscr, maxY, maxX);
  if (rows == 0 && cols == 0) {
    render_frame(maxY, maxX);
  } else {
    int startX = (maxX - cols) / 2;
    int startY = (maxY - rows) / 2;
    for (size_t x = 0; x < cols; x++) {
      mvaddch(startY, startX + x, '-');    
      mvaddch(startY + rows - 1, startX + x, '-'); 
    }
    for (int y = 1; y < rows - 1; y++) {
      mvaddch(startY + y, startX, '|');
      mvaddch(startY + y, startX + cols - 1, '|');
    }
    mvaddch(startY, startX, '+');
    mvaddch(startY, startX + cols - 1, '+');
    mvaddch(startY + rows - 1, startX, '+');
    mvaddch(startY + rows - 1, startX + cols - 1, '+');
    refresh();
  }
}

void render_interface(menu_t *menu) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    clear();
    render_frame(menu->currOptionCnt + 4, menu->longestEntry + 8);

    /* 1) HELP sa kreslí hore, ale NEKONČÍME funkciu (menu sa má zobraziť tiež) */
    int helpCount = 0;
    if (menu->helpShown) {
        const char *const *lines = NULL;
        menu_get_help(&lines, &helpCount); /* len aby sme vedeli, kolko riadkov zaberie HELP */
        render_help(menu);
    }

    /* 2) Menu položky */
    const char *const *items = NULL;
    get_menu_items(menu, &items);
    const game_setup_t *setup = menu_get_setup(menu);

    int xStart = (cols - menu->longestEntry) / 2;
    int yStart = (rows - menu->currOptionCnt) / 2;
    /* 4) Vykreslenie menu */
    for (int j = 0; j < menu->currOptionCnt; ++j) {
        int x = (menu->selectedOptionId == j) ? (xStart - 4) : xStart;

        if (menu->selectedOptionId == j) {
            mvprintw(yStart + j, x, "==> ");
            x += 4;
        }

        if (menu->currMenu != GAME_WIZARD) {
            mvprintw(yStart + j, x, "%s", items[j]);
            continue;
        }

        switch (j) {
            case 0: mvprintw(yStart + j, x, "%s%s", items[j], (setup && setup->timed == 0) ? " [X]" : " [ ]"); break;
            case 1: mvprintw(yStart + j, x, "%s%s", items[j], (setup && setup->timed == 1) ? " [X]" : " [ ]"); break;
            case 2: mvprintw(yStart + j, x, "%s %d", items[j], setup ? setup->players : 0); break;
            case 3: mvprintw(yStart + j, x, "%s %d", items[j], setup ? setup->world_w : 0); break;
            case 4: mvprintw(yStart + j, x, "%s %d", items[j], setup ? setup->world_h : 0); break;
            case 5: mvprintw(yStart + j, x, "%s %s", items[j], (setup && setup->obstacles) ? "ON" : "OFF"); break;
            default: mvprintw(yStart + j, x, "%s", items[j]); break;
        }
    }

    refresh();
}


void render_help(menu_t *menu) {
  (void)menu; //aby kompilator neziapal: Unused parameter menu...

  const char *const *lines = NULL;
  int count = 0;
  menu_get_help(&lines, &count);

  int y0 = 1;
  int x0 = 2;

  for (int i = 0; i < count; ++i) {
    mvprintw(y0 + i, x0, "%s", lines[i]);
  }
}


void render_message(char *message) {
  int maxX, maxY;
  int messLen = strlen(message);
  getmaxyx(stdscr, maxY, maxX);
  mvprintw(3, maxX - (3 +messLen), "%s", message);
}

void destroy_display(display_t *display) {
  display->currGame = NULL;
  display->window = 0;
  endwin();
}


