#include <unistd.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h> //malloc
#include <string.h> //memset

#define ARR_EXP_CONST 5

enum Direction {
  STATIC,
  UP,
  DOWN,
  LEFT,
  RIGHT
};

typedef struct {
  int xPos;
  int yPos;
} position_t;

void position_init(position_t *pos, int x, int y) {
  (*pos).xPos = x;
  (*pos).yPos = y;
}

typedef struct {
} game_object_t;

void update_positions(position_t* pos, size_t length, int diff, char coordId, int rows, int cols) {
  for (int pid = length - 1; pid >= 0; --pid) {
    if  (pid == 0) {
      if (coordId == 'x') {
        pos[pid].xPos += diff;
      } else {
        pos[pid].yPos += diff;
      }
    } else {
      position_init(&pos[pid], pos[pid - 1].xPos, pos[pid - 1].yPos);
    }
    if (pos[pid].yPos < 0) {
      pos[pid].yPos = rows - 1;
    } else if (pos[pid].yPos >= rows) {
      pos[pid].yPos = 0;
    }
    if (pos[pid].xPos < 0) {
      pos[pid].xPos = cols - 1;
    } else if (pos[pid].xPos >= cols) {
      pos[pid].xPos = 0;
    }
  }
}

int main() {
  initscr();
  start_color();
  noecho();
  curs_set(FALSE);
  keypad(stdscr, TRUE);
  timeout(50);
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);
  attron(COLOR_PAIR(1));
  int points = 0;
  int currPosArrLen = ARR_EXP_CONST;
  position_t *positions = calloc(currPosArrLen, sizeof(position_t));
  position_t prevPos = {0, 0};
  position_init(&positions[0], 5, 5);
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  enum Direction currDirection = STATIC;
  position_init(&prevPos, positions[0].xPos, positions[0].yPos);
  size_t nOO= 6;
  position_t objects[nOO];
  int objX = 20;
  attron(COLOR_PAIR(2));
  for (size_t i = 0; i < nOO; ++i) {
    position_init(&objects[i], objX, 20);
    mvaddch(objects[i].yPos, objects[i].xPos, '$');
   objX += 10;
  }
  attroff(COLOR_PAIR(2));
  int command = -1;
  mvprintw(5, cols/2 - 27, "Maximalne suradnice su: %d, %d", rows, cols);
  while(1) {
      command = getch(); //niektore kody klaves su vacsie ako 255
      attroff(COLOR_PAIR(1)); 
      mvprintw(2, cols - 12, "Score: %d", points);
      attron(COLOR_PAIR(1));
      mvprintw(5, cols - 20, "Stlacene: %d", command);
      if (prevPos.xPos != -1) {
        mvaddch(prevPos.yPos, prevPos.xPos, ' ');
      }
      mvaddch(positions[0].yPos, positions[0].xPos, 'O');
//      if (points - 1 >= 0) {
//        position_init(&prevPos, positions[points - 1].xPos, positions[points - 1].yPos); 
//      } else {
        position_init(&prevPos, positions[points].xPos, positions[points].yPos);
//      }
      refresh();
      // spracovanie stlacenej klavesy
      if (command == KEY_DOWN && currDirection != UP) {
          currDirection = DOWN;
      } else if (command == KEY_UP && currDirection != DOWN) {
          currDirection = UP;
      } else if (command == KEY_LEFT && currDirection != RIGHT) {
          currDirection = LEFT;
      } else if (command == KEY_RIGHT && currDirection != LEFT) {
          currDirection = RIGHT;
      } else if (command == 27) { // ESC
          endwin();
          break;
      } else if (command == 32) {
        while(1) {
          command = getch();
          if (command == 32) {
            break;
          }
        }
      } else {
          // pohyb podla posledneho smeru
          if (currDirection == UP) {
              update_positions(positions, points + 1, -1, 'y', rows, cols);
          } else if (currDirection == DOWN) {
              update_positions(positions, points + 1, 1, 'y', rows, cols);
          } else if (currDirection == LEFT) {
              update_positions(positions, points + 1, -1, 'x', rows, cols);
          } else if (currDirection == RIGHT) {
              update_positions(positions, points + 1, 1, 'x', rows, cols);
          }
          usleep(70000);
      } 
      for (size_t oid = 0; oid < nOO; ++oid) {
        if (objects[oid].xPos > -1 && positions[0].xPos == objects[oid].xPos && positions[0].yPos == objects[oid].yPos) { 
            objects[oid].xPos = -1;
          points++;
          if (points >= currPosArrLen) {
            position_t *tmpPositions = realloc(positions, (currPosArrLen + ARR_EXP_CONST) * sizeof(position_t));
            if (tmpPositions == NULL) {
              perror("realloc (positions)!");
            } else {
              positions = tmpPositions;
              currPosArrLen += ARR_EXP_CONST;
            } 
          }
          position_init(&positions[points], prevPos.xPos, prevPos.yPos);
          prevPos.xPos = -1;
        }
      }
  }
  free(positions); 
  return 0;
}
