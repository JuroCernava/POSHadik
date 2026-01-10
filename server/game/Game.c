#include "Game.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define OBSTACLE_CNT 6
#define TICK_USEC 70000

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

//konflikt s inym hracom
static _Bool is_on_any_player(const game_t *game, const position_t *pos) {
    for (size_t p = 0; p < game->playerCnt; ++p) {
        const player_t *pl = &game->players[p];
        if (!pl->living) continue;

        int len = pl->score + 1;
        for (int i = 0; i < len; ++i) {
            if (pl->positions[i].xPos == pos->xPos &&
                pl->positions[i].yPos == pos->yPos) {
                return 1;
            }
        }
    }
    return 0;
}

static world_corner_t *make_corners_from_setup(const game_setup_t *s) {
    world_corner_t *c = malloc(sizeof(world_corner_t));
    // predpoklad: (0,0) je ok, alebo to prispôsob svojmu UI rámu
    c->startX = 0;
    c->startY = 0;
    c->endX   = s->world_w - 1;
    c->endY   = s->world_h - 1;
    return c;
}

void g_settings_from_setup(const game_setup_t *s, g_settings_t *settings) {
    settings->width  = s->world_w;
    settings->height = s->world_h;
    settings->obstacles = (s->obstacles != 0);
    settings->mode = (s->timed != 0) ? TIMED : STANDARD;
    settings->time = (int)s->time; // ak je s->time v minútach
    settings->corners = make_corners_from_setup(s);
}

static _Bool is_on_object(const game_t *game, const position_t *pos) {
  for (size_t oid = 0; oid < game->objectsCnt; ++oid ) {
    if (pos->xPos == game->objects[oid].position.xPos &&
        pos->yPos == game->objects[oid].position.yPos) {
      return 1;
    }
  }
  return 0;
}

static _Bool is_occupied(const game_t *game, const position_t *pos) {
    return is_on_object(game, pos) || is_on_any_player(game, pos);
}

static void game_generate_object(game_t *game, ObjectType objType, game_object_t *object) {
    for (;;) {
        int randX = game->settings.corners->startX + 1 + (rand() % game->settings.width);
        int randY = game->settings.corners->startY + 1 + (rand() % game->settings.height);

        position_t testPos;
        position_init(&testPos, randX, randY);

        if (!is_occupied(game, &testPos)) {
              object->objectType = objType;
              object->position.xPos = randX;
            object->position.yPos = randY;
            return;
        }
    }
}

static void game_add_obstacles(game_t *game) {
    for (size_t i = 0; i < OBSTACLE_CNT; ++i) {
        game_generate_object(game, OBSTACLE, &game->objects[game->objectsCnt]);
        game->objectsCnt++;
    }
}

  static void game_add_food_for_players(game_t *game) {
    for (size_t j = 0; j < game->playerCnt; ++j) {
        game_generate_object(game, FOOD, &game->objects[game->objectsCnt]);
        game->objectsCnt++;
    }
}

static _Bool head_hits_obstacle(const game_t *game, size_t playerId) {
    const position_t *h = &game->players[playerId].positions[0];
    for (size_t oid = 0; oid < game->objectsCnt; ++oid) {
        if (game->objects[oid].objectType != OBSTACLE) continue;
        if (game->objects[oid].position.xPos == h->xPos &&
            game->objects[oid].position.yPos == h->yPos) {
            return 1;
        }
    }
    return 0;
}

static _Bool head_hits_any_snake(const game_t *game, size_t playerId) {
    const player_t *me = &game->players[playerId];
    const position_t *h = &me->positions[0];

    for (size_t p = 0; p < game->playerCnt; ++p) {
        const player_t *pl = &game->players[p];
        if (!pl->living) continue;

        int len = pl->score + 1;
        for (int i = 0; i < len; ++i) {
            if (p == playerId && i == 0) 
              continue;
            if (pl->positions[i].xPos == h->xPos &&
                pl->positions[i].yPos == h->yPos) {
                return 1;
            }
        }
    }
    return 0;
}

static int found_food(game_t *game, int pId) {  //find_eaten_food_index
    const position_t *h = &game->players[pId].positions[0];
    for (size_t oid = 0; oid < game->objectsCnt; ++oid) {
        if (game->objects[oid].objectType != FOOD) continue;
        if (game->objects[oid].position.xPos == h->xPos &&
            game->objects[oid].position.yPos == h->yPos) {
            return (int)oid;
        }
    }
    return -1;
}

static _Bool any_living(game_t *game) {
  for (size_t i = 0; i < game->playerCnt; ++i) {
    if (game->players[i].living) {
      return 1;
    }
  }
  return 0;
}

static void g_settings_init(g_settings_t *dst, const g_settings_t *src) {
    dst->mode = src->mode;
    dst->height = src->height;
    dst->width = src->width;
    dst->time = src->time;
    dst->obstacles = src->obstacles;

    if (src->corners) {
        dst->corners = malloc(sizeof(world_corner_t));
        *dst->corners = *src->corners;
    } else {
        dst->corners = NULL;
    }
}

void game_init(game_t *game, g_settings_t *settings) {
    game->playerCap = 3;
    game->playerCnt = 1;

    g_settings_init(&game->settings, settings);

    game->players = malloc(game->playerCnt * sizeof(player_t));
    player_init(&game->players[0], 0, game->settings.width / 2, game->settings.height / 2);
    size_t obstacles = game->settings.obstacles ? OBSTACLE_CNT : 0;
    size_t foods = game->playerCnt;
    game->objectsCnt = 0;
    game->objectsCap = obstacles + foods;              // <- ADD this field to game_t
    game->objects = malloc(game->objectsCap * sizeof(game_object_t));
    if (game->settings.obstacles) {
      game_add_obstacles(game);
    }
    game_add_food_for_players(game);
    game->running = 0;
}

void game_update_p_direction(game_t *game, int pId, Direction newDir) {
  if (game->players[pId].living && game->players[pId].running &&
      newDir != opposite(game->players[pId].direction)) {
      game->players[pId].direction = newDir;
  } 
}

void game_pause(game_t *game, unsigned char playerId) {
  player_pause(&game->players[playerId]);
  update_player_direction(&game->players[playerId], STATIC);
}

void game_to_snap(game_t *game, world_snap_t *snap) {
    snap->playerCnt = game->playerCnt;

    // PLAYERS
    snap->playerScores = malloc(snap->playerCnt * sizeof(int)); 

    int totalSegs = 0;
    for (size_t p = 0; p < snap->playerCnt; ++p) {
        snap->playerScores[p] = game->players[p].score; //
        totalSegs += snap->playerScores[p] + 1; //
    }

    snap->pSegments = malloc(totalSegs * sizeof(position_t));

    int idx = 0;
    for (size_t p = 0; p < snap->playerCnt; ++p) {
        player_t *pl = &game->players[p];
        for (int s = 0; s <= pl->score; ++s) {
            snap->pSegments[idx++] = pl->positions[s];
        }
      }

    // OBJECTS
    snap->obstacleCnt = 0;
    for (size_t i = 0; i < game->objectsCnt; ++i) {
        if (game->objects[i].objectType == OBSTACLE)
            snap->obstacleCnt++;
    }

    snap->obstaclePos = malloc(snap->obstacleCnt * sizeof(position_t));
    snap->foodPos = malloc(snap->playerCnt * sizeof(position_t));

    int oi = 0, fi = 0;
    for (size_t i = 0; i < game->objectsCnt; ++i) {
        if (game->objects[i].objectType == OBSTACLE) {
            snap->obstaclePos[oi++] = game->objects[i].position;
        } else if (game->objects[i].objectType == FOOD) {
            if (fi < snap->playerCnt) {
              snap->foodPos[fi++] = game->objects[i].position;
            }
        }

    }
}

//GAME LOOP s pomocnou...
static _Bool cq_try_pop(command_queue_t *q, int *out)
{
    _Bool ok = 0;
    pthread_mutex_lock(&q->mutex);

    if (q->commandCnt > 0) {
        *out = q->commands[q->outId];
        q->outId = (q->outId + 1) % q->cap;
        q->commandCnt--;
        ok = 1;
    }

    pthread_mutex_unlock(&q->mutex);
    return ok;
}

void* game_run(void *args) {
  game_args_t *gameArgs = args;
  game_t *game = gameArgs->game;
  world_snap_t *currSnap = gameArgs->snap;
  game->running = 1;
  double start = now_seconds();
  double noLivingSince = 0.0;
  socket_data_t *activeSocket = gameArgs->activeSocket;
  while (game->running) {
    int currCommand; 
    while (cq_try_pop(&gameArgs->commands, &currCommand)) {
      switch (currCommand) {
        case 0: update_player_direction(&game->players[0], UP); break;
        case 1: update_player_direction(&game->players[0], DOWN); break;
        case 2: update_player_direction(&game->players[0], LEFT); break;
        case 3: update_player_direction(&game->players[0], RIGHT); break;
      }
    }
    // 1) move living players
    for (size_t p = 0; p < game->playerCnt; ++p) {
        if (!game->players[p].living) continue;
        update_player_positions(&game->players[p], game->settings.height, game->settings.width);
    }
    // 2) collisions + food
    for (size_t p = 0; p < game->playerCnt; ++p) {
        if (!game->players[p].living) continue;

        if (head_hits_obstacle(game, p) || head_hits_any_snake(game, p)) {
            player_destroy(&game->players[p]); // living=0 + free positions (per your implementation)
            continue;
        }

        int foodId = found_food(game, p);
        if (foodId >= 0) {
            player_grow(&game->players[p]);

              // respawn this food somewhere else
            game_generate_object(game, FOOD, &game->objects[foodId]);
        }
    }
    // 3) stop condition
    if (game->settings.mode == TIMED) {
        if ((now_seconds() - start) * 60 >= (double)game->settings.time) {
            game->running = 0;
        }
    } else {
        if (any_living(game)) {
            noLivingSince = 0.0;
        } else {
            if (noLivingSince == 0.0) noLivingSince = now_seconds();
            if (now_seconds() - noLivingSince >= 10.0) {
                game->running = 0;
            }
        }
    }
    game_to_snap(game, currSnap);
    socket_write(activeSocket, (const char *)currSnap, sizeof(*currSnap));
    snap_destroy(currSnap);
    usleep(TICK_USEC);
  }
  shutdown(activeSocket->socket, SHUT_RDWR);
  socket_destroy(activeSocket);
  return NULL;
}

void snap_destroy(world_snap_t *s) {
    free(s->playerScores);
    free(s->foodPos);
    free(s->obstaclePos);
    free(s->pSegments);
    memset(s, 0, sizeof(*s));
}
