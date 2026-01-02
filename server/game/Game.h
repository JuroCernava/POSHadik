#ifndef GAME_H
#define GAME_H

enum GameMode {
  STANDARD,
  TIMED
}; 
typedef struct {

} game_t;
void game_init(game_t *this, int height, int width, int time, socket_t serverSocket);


#endif

