#include "ActionManager.h"
#include <ncurses.h>

void action_manager_init(action_manager_t *manager) {
  manager->state = AM_MENU;
}

int handle_event(int keyCode, action_manager_t *manager, menu_t *menu) {
  if (manager->state == AM_MENU) {
    switch (keyCode) {
      case KEY_UP:
        menu_prev(menu);
        return 0;
      case KEY_DOWN:
        menu_next(menu);
        return 0;
      // ak by ENTER nefungoval treba zmenit na KEY_RIGHT
      case 10:  //'\n'
      case 13:  //'\r'
      case KEY_RIGHT: // PRE ISTOTU... 
        menu_enter(menu);
        if (menu->currMenu == GAME_RUNNING) {
          manager->state = AM_GAME;
        }
        return 0;
      default:
        return -1;
    }
  } else if (manager->state == AM_GAME) {
    switch(keyCode) {
      case 32:
      case 10:
      case 13:
      case KEY_RIGHT:
        manager->state = AM_PAUSED;
        return 0;
      default:
        return -1;
    }
  } else if (manager->state == AM_PAUSED) {
    switch(keyCode) {
      case 32:
      case 10:
      case 13:
      case KEY_RIGHT:
        menu_enter(menu);
        manager->state = AM_MENU;
        return 0;
      case 27:
        manager->state = AM_GAME;
        return 0;
      default:
        return -1;
    }
  } else {
    return -2;
  }
}

void message_to_action(int message, action_t *action) {
  (void)message;
  (void)action;
}

void action_manager_destroy(action_manager_t *manager) {
  (void)manager;
}
