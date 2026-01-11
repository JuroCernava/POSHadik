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

    game_t game = {0}; //secko na NULL/0
    socket_server_t srvSocket = {0};
    server_t server = {0};

    server.game = &game; 
    server.socket = &srvSocket; 

    server_init(&server, port);
    server_run_game(&server);
    server_destroy(&server);

    return 0;
}
