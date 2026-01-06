#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include "../menu/Menu.h"

typedef enum {
  AM_MENU,
  AM_GAME,
  AM_PAUSED
} ActionManagerState;

typedef enum {
  USER_MOVE_UP,
  USER_MOVE_DOWN,
  USER_MOVE_LEFT,
  USER_MOVE_RIGHT,
  USER_ENTER,
  SCORE_CHANGE,
  OBJECT_CHANGE,
  PLAYER_CHANGE
} ChangeType;

typedef struct {
  ActionManagerState state;
} action_manager_t;

typedef struct {
  ChangeType type;
  //position_t *posStart;
  //position_t *posEnd;
} action_t;

void action_manager_init(action_manager_t *manager);
int handle_event(int keyCode, action_manager_t *manager, menu_t *menu);
void message_to_action(int message, action_t *action);
void action_manager_destroy(action_manager_t *manager);

#endif
