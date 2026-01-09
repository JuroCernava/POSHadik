#include "Client.h"
#include "action manager/ActionManager.h"
#include "menu/Menu.h"
#include "renderer/Renderer.h"
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void client_init(client_t *client) {
  srand((unsigned)time(NULL));
  action_manager_init(&client->actionManager);
  client->gameId = -1;   
  client->playerId = -1;     
  init_display(&client->display);
  world_corner_t corners;
  render_frame(client->menu.currOptionCnt + 2, client->menu.longestEntry + 6, &corners);
  menu_init(&client->menu);
  render_interface(&client->menu);
}

void client_create_game(client_t *client, int height, int width, GameMode mode, _Bool obstacles) {
  pid_t pid = fork();

  if (pid == 0) {
    /* child → server */
    execl("./server", "./server", "12345", NULL);
    perror("exec server");
    exit(1);
  }

}

//typedef struct {
//  world_corner_t *corners;
//  GameMode mode;
//  int height;
//  int width;
//  int time;
//  _Bool obstacles;
  //} g_settings_t;

static world_corner_t *make_corners_from_setup(const game_setup_t *s) {
    world_corner_t *c = malloc(sizeof(world_corner_t));
    // predpoklad: (0,0) je ok, alebo to prispôsob svojmu UI rámu
    c->startX = 0;
    c->startY = 0;
    c->endX   = s->world_w - 1;
    c->endY   = s->world_h - 1;
    return c;
}

g_settings_t g_settings_from_setup(const game_setup_t *s) {
    g_settings_t gs;
    gs.width  = s->world_w;
    gs.height = s->world_h;
    gs.obstacles = (s->obstacles != 0);
    gs.mode = (s->timed != 0) ? TIMED : STANDARD;
    gs.time = (int)s->time; // ak je s->time v minútach
    gs.corners = make_corners_from_setup(s);
    return gs;
}

void client_listen(client_t *client) {
  keypad(stdscr, TRUE);
  world_corner_t corners;
  game_t currGame;
  g_settings_t gSettings;
  while (1) {
    int keyPressed = getch();
    int newAction = handle_event(keyPressed, &client->actionManager, &client->menu);
    if (newAction == 0) {
      if (client->menu.currMenu == APP_EXITED) {
        endwin();
        break;
      } else {
        clear();
        render_frame(client->menu.currOptionCnt + 2, client->menu.longestEntry + 6, &corners);
        if (client->actionManager.state == AM_GAME) {
          gSettings = g_settings_from_setup(&client->menu.setup);
          game_init(&currGame, &gSettings);
          game_run(&currGame, &gSettings);
          render_game_world(&client->menu);
        } else {
          render_interface(&client->menu);
        } 
      }
    } else {
      render_message("Stlacene zle tlacidlo.");
    }
  }
}

void client_destroy(client_t *client) {
  (void)client;
}

