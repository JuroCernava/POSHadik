#include <stdlib.h> 
#include <stdio.h>
#include "../shared/socket/Socket.h"
#include "Server.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: %s <port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);
    game_t game;
    server_t runServer;
    socket_server_t srv;
    socket_server_init(&srv, port);

    printf("[SERVER] Pocuvam na porte %d...\n", port);
    socket_server_accept_connection(&srv);
    printf("[SERVER] Klient pripojeny.\n");

    game_setup_t msg;
    socket_read(&srv.activeSocket, (char*)&msg, sizeof(msg)); 
    socket_server_destroy(&srv);
    return 0;
}
