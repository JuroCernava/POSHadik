#include "Client.h"
#include "action manager/ActionManager.h"
#include "menu/Menu.h"
#include "renderer/Renderer.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define UTICK_TIME 70000
#define OBSTACLE_CNT 6

//helper
static int read_exact(socket_data_t *s, void *buf, size_t n) {
    size_t off = 0;
    while (off < n) {
        //POSIX metoda na citanie B z filedescriptoru vrati < 0 - chyba, 0 - EOF, > 0 - pocet skutocne precitanych B
        //znamena ze cita az kym neprecita celu velkost objektu
        int r = read(s->socket, (char*)buf + off, n - off);
        if (r <= 0) return r; // 0=EOF, <0=error
        off += (size_t)r;
    }
    return 1;
}

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

void client_init_game_state(game_state_t *state, size_t snapCap) {
  state->inId = 0;
  state->outId = 0;
  state->cnt = 0;
  state->snapCap = snapCap;
  state->snaps = malloc(snapCap * sizeof(world_snap_t));
  pthread_mutex_init(&state->mutex, NULL);
}


void* client_read_game_snaps(void *args) {
    recv_data_t *r = args;
    for (;;) {
        world_snap_hdr_t h; 
        int ok = read_exact(r->socket, &h, sizeof(h));
        if (ok <= 0) break;

        world_snap_t snap;
        memset(&snap, 0, sizeof(snap));

        snap.playerCnt = (size_t)h.playerCnt;

        // alokácie podľa headeru
        snap.playerLen = malloc(snap.playerCnt * sizeof(size_t));
        snap.foodPos   = malloc(snap.playerCnt * sizeof(position_t));
        snap.pSegments = malloc((size_t)h.segmentCnt * sizeof(position_t));

        if (!snap.playerLen || !snap.foodPos || !snap.pSegments) {
            snap_destroy(&snap);
            break;
        }

        ok = read_exact(r->socket, snap.playerLen, snap.playerCnt * sizeof(size_t));
        if (ok <= 0) { snap_destroy(&snap); break; }

        ok = read_exact(r->socket, snap.foodPos, snap.playerCnt * sizeof(position_t));
        if (ok <= 0) { snap_destroy(&snap); break; }

        ok = read_exact(r->socket, snap.obstPos, OBSTACLE_CNT * sizeof(position_t));
        if (ok <= 0) { snap_destroy(&snap); break; }

        ok = read_exact(r->socket, snap.pSegments, (size_t)h.segmentCnt * sizeof(position_t));
        if (ok <= 0) { snap_destroy(&snap); break; }


        //KS
        pthread_mutex_lock(&r->state->mutex);

        snap_destroy(&r->state->snaps[r->state->inId]);

        r->state->snaps[r->state->inId] = snap;
        r->state->inId = (r->state->inId + 1) % r->state->snapCap;
        if (r->state->cnt < r->state->snapCap) r->state->cnt++;

        pthread_mutex_unlock(&r->state->mutex);
    }
    return NULL;
}

static pid_t start_server_exec(int port) {
  pid_t pid = fork();
  if (pid < 0) return -1;

  if (pid == 0) {
      char portStr[16];
      snprintf(portStr, sizeof(portStr), "%d", port);
      execl("./server/serverApp", "serverApp", portStr, (char*)NULL);
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

void client_send_command(int command, socket_data_t *socket) {
  socket_write(socket, (const char*) &command, sizeof(int));
}

static _Bool try_pop_latest(game_state_t *st, world_snap_t *out) {
    _Bool ok = 0;

    pthread_mutex_lock(&st->mutex);

    if (st->cnt > 0) {
        // posledný je na indexe (inId - 1)
        size_t lastId = (st->inId + st->snapCap - 1) % st->snapCap;
        *out = st->snaps[lastId];

        // vyprázdni front (zahod všetko staré)
        st->outId = st->inId;
        st->cnt = 0;

        ok = 1;
    }

    pthread_mutex_unlock(&st->mutex);
    return ok;
}

void client_listen(client_t *client) {
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    world_corner_t corners;
    const int PORT = 6666;

    _Bool serverStarted = 0;
    _Bool inGameConn = 0;

    socket_client_t gameCl = {0};
    game_state_t state = {0};
    recv_data_t recvArgs = {0};
    pthread_t recvThread;

    while (1) {
        int keyPressed = getch();
        int action = handle_event(keyPressed, &client->actionManager, &client->menu);

        // exit aplikácie
        if (action == 0 && client->menu.currMenu == APP_EXITED) {
            endwin();
            break;
        }

        // ak sme v hre
        if (client->actionManager.state == AM_GAME) {
            if (!serverStarted) {
                start_server_exec(PORT);
                usleep(300000); // 300ms nech server stihne bind/listen
                serverStarted = 1;
            }

            // pripojenie len raz
            if (!inGameConn) {
                char portStr[16];
                snprintf(portStr, sizeof(portStr), "%d", PORT);

                // pripoj klienta (socket_client_init u teba vracia _Bool)
                _Bool connected = 0;
                for (int i = 0; i < 50; ++i) {          // krátky retry
                    connected = socket_client_init(&gameCl, "127.0.0.1", portStr);
                    if (connected) break;
                    usleep(20000);
                }
                if (!connected) {
                    render_message("Nepodarilo sa pripojit na server.");
                    client->actionManager.state = AM_MENU;
                    continue;
                }

                // pošli setup (na tom istom spojení!)
                game_setup_t msg = {
                    .timed     = client->menu.setup.timed,
                    .obstacles = client->menu.setup.obstacles,
                    .time      = client->menu.setup.time,
                    .world_h   = client->menu.setup.world_h,
                    .world_w   = client->menu.setup.world_w,
                    .players   = client->menu.setup.players
                };
                socket_write(&gameCl.activeSocket, (const char*)&msg, sizeof(msg));

                // init ring buffer na snapy + recv thread
                client_init_game_state(&state, 16);
                recvArgs.state = &state;
                recvArgs.socket = &gameCl.activeSocket;
                pthread_create(&recvThread, NULL, client_read_game_snaps, &recvArgs);

                inGameConn = 1;
            }

            // posielanie príkazov (ActionManager: 1..4)
            // server číta 1 byte a robí cmd = ch - '0', cmd v 0..3
            if (action >= 1 && action <= 4) {
                char ch = (char)('0' + (action - 1));   // 1->'0', 2->'1', 3->'2', 4->'3'
                socket_write(&gameCl.activeSocket, &ch, 1);
            }
            clear();
            render_frame(client->menu.setup.world_h, client->menu.setup.world_w, &corners);
            
            world_snap_t latest;
            //render_world_from_snap(const menu_t *menu, const world_snap_t *snap)
            if (try_pop_latest(&state, &latest)) {
                render_world_from_snap(&client->menu, &latest);
            }
                refresh();
            }
        // mimo hry (menu/pause)
        else {
            // keď odídeš z AM_GAME, zatvor spojenie a ukonči thread
            if (inGameConn) {
                shutdown(gameCl.activeSocket.socket, SHUT_RDWR);
                pthread_join(recvThread, NULL);
                socket_client_destroy(&gameCl);
                inGameConn = 0;
                serverStarted = 0; // ak chceš, aby sa server spúšťal znova pri ďalšej hre
            }
            render_interface(&client->menu);
        }

        usleep(UTICK_TIME);
    }
}

void client_destroy(client_t *client) {
  (void)client;
}

