#include "Server.h"
#include "game/Game.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "../shared/socket/Socket.h"

static void commands_init(command_queue_t *q, size_t cap) {
    q->cap = cap;
    q->commands = (int*)malloc(cap * sizeof(int));
    q->commandCnt = 0;
    q->inId = 0;
    q->outId = 0;
    pthread_mutex_init(&q->mutex, NULL);
}

//“Passive socket slúži len na accept, aktívny socket reprezentuje konkrétne spojenie a cez neho prebieha read/write komunikácia.”

void server_init(server_t *server, int port) {
  server->running = 1;
  socket_server_init(server->socket, port);
  printf("[SERVER] Pocuvam na porte %d...\n", port);
  socket_server_accept_connection(server->socket);
  printf("[SERVER] Klient pripojeny.\n");
  game_setup_t gSetup;
  socket_read(&server->socket->activeSocket, (char*)&gSetup, sizeof(gSetup));
  g_settings_t gSettings;
  g_settings_from_setup(&gSetup, &gSettings);
  game_init(server->game, &gSettings);
}

void* server_recv_command(void* args) {
    recver_t *r = args;

    while (1) {
        char ch = 0;
        socket_read(r->soc, &ch, 1);

        // keď game zavrie socket cez socket_destroy(), sock->socket sa nastaví na -1
        if (!socket_is_valid(r->soc)) break;

        int cmd = ch - '0';
        if (cmd < 1 || cmd > 4) continue;

        // push drop (bez cond)
        pthread_mutex_lock(&r->commands->mutex);
        if (r->commands->commandCnt < r->commands->cap) {
            r->commands->commands[r->commands->inId] = cmd;
            r->commands->inId = (r->commands->inId + 1) % r->commands->cap;
            r->commands->commandCnt++;
        }
        pthread_mutex_unlock(&r->commands->mutex);
    }

    return NULL;
}

static void commands_destroy(command_queue_t *q) {
    pthread_mutex_destroy(&q->mutex);
    free(q->commands);
}

void server_run_game(server_t *server) {
    game_args_t gameArgs;
    gameArgs.activeSocket = &server->socket->activeSocket;
    gameArgs.game = server->game;

    commands_init(&gameArgs.commands, 10);

    recver_t receiverArgs;
    receiverArgs.commands = &gameArgs.commands;                 // <- POINTER (fix)
    receiverArgs.soc = &server->socket->activeSocket;      // <- ACTIVE (fix)

    pthread_t t_game, t_recv;

    // 1) spusti recv thread
    pthread_create(&t_recv, NULL, server_recv_command, &receiverArgs);

    // 2) spusti game thread
    pthread_create(&t_game, NULL, game_run, &gameArgs);

    // 3) počkaj na koniec hry
    pthread_join(t_game, NULL);

    // 4) game_run po skončení zavrie socket (shutdown+socket_destroy),
    //    tým sa recv thread odblokuje a skončí
    pthread_join(t_recv, NULL);
    commands_destroy(&gameArgs.commands);
}


void server_destroy(server_t *server) {
  socket_server_destroy(server->socket);
  game_destroy(server->game);
  server->running = 0;
}
