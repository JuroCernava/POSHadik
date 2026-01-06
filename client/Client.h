#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>
#include "./menu/Menu.h"
#include "./action manager/ActionManager.h"
#include "./renderer/Renderer.h"

typedef struct {
  int playerId;
    int gameId;

  action_manager_t actionManager;
  pthread_mutex_t mutex;

  int *messages;
  display_t display;
  menu_t menu;
} client_t;

void client_init(client_t *client);
void client_listen(client_t *client);
void client_destroy(client_t *client);

#endif
