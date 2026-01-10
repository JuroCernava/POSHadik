#include "Client.h"
#include "action manager/ActionManager.h"
#include "menu/Menu.h"
#include "renderer/Renderer.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "../shared/socket/Socket.h"

static void send_setup_to_server(const menu_t *menu, int port) {
    socket_client_t cl;
    char portStr[16];
    snprintf(portStr, sizeof(portStr), "%d", port);
    _Bool connected = 0;

    for (size_t i = 0; i < 100; ++i) {
      connected = socket_client_init(&cl, "127.0.0.1", portStr);
      if (connected) {
        break;
      }
    }
    game_setup_t msg = {
        .timed     = menu->setup.timed,
        .obstacles = menu->setup.obstacles,
        .time      = menu->setup.time,
        .world_h   = menu->setup.world_h,
        .world_w   = menu->setup.world_w,
        .players   = menu->setup.players
    };

    socket_write(&cl.activeSocket, (const char*)&msg, sizeof(msg));
    socket_client_destroy(&cl);
}

static pid_t start_server_exec(int port) {
    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        char portStr[16];
        snprintf(portStr, sizeof(portStr), "%d", port);
        execl("./server/serverApp", "serverApp", "6666", (char*)NULL);
        _exit(127); // execl zlyhal
    }
    return pid; // parent dostane pid servera
}

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

//typedef struct {
//  world_corner_t *corners;
//  GameMode mode;
//  int height;
//  int width;
//  int time;
//  _Bool obstacles;
  //} g_settings_t;

void client_listen(client_t *client) {
  keypad(stdscr, TRUE);
  world_corner_t corners;
  _Bool serverStarted = 0;
  _Bool gameSetupSent = 0;
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
          if (!serverStarted) {
            start_server_exec(6666);
            render_message("Spustam server...\n");
            usleep(300000); // cakanie na spustenie servera 300ms
            serverStarted = 1;
          }
          if (serverStarted && !gameSetupSent) {
            send_setup_to_server(&client->menu, 6666);
            gameSetupSent = 1;
          }  
          render_game_world(&client->menu);
        } else {
          serverStarted = 0;
          gameSetupSent = 0;
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

