#include "Client.h"
#include "renderer/Renderer.h"
#include <ncurses.h>

void client_init(client_t *client) {
  action_manager_init(&client->actionManager);
  client->gameId = -1;   
  client->playerId = -1;     
  init_display(&client->display);
  render_frame(client->menu.currOptionCnt + 2, client->menu.longestEntry + 6);
  menu_init(&client->menu);
  render_interface(&client->menu);
}

void client_listen(client_t *client) {
  keypad(stdscr, TRUE);
  while (1) {
    int keyPressed = getch();
    int newAction = handle_event(keyPressed, &client->actionManager, &client->menu);
    if (newAction == 0) {
      if (client->menu.currMenu == APP_EXITED) {
        endwin();
        break;
      } else {
        clear();
        render_frame(client->menu.currOptionCnt + 2, client->menu.longestEntry + 6);
        render_interface(&client->menu);
      }
    } else {
      render_message("Stlacene zle tlacidlo.");
    }
  }
}

void client_destroy(client_t *client) {
  (void)client;
}

