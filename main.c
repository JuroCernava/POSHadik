#include <ncurses.h>
#include <unistd.h>
enum Direction {
  STATIC,
  UP,
  DOWN,
  LEFT,
  RIGHT
};


int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    timeout(50);
    int currX = 5;
    int currY = 5;
    int prevX = 5;
    int prevY = 5;
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    enum Direction currDirection = STATIC;
  while(1) {
    int command = getch(); //klavesy ako napr. sipky maju vacsie hodnoty ako 255...
    mvprintw(prevY, prevX, " ");
    mvprintw(currY, currX, "O");
    refresh();
    prevY = currY;
    prevX = currX;
    if (command == KEY_DOWN) {
      currY++;
      erase();
      currDirection = DOWN;
    } else if (command == KEY_UP) {
      currY--;
      currDirection = UP;
    } else if (command == KEY_LEFT) {
      currX--;
      currDirection = LEFT;
    } else if (command == KEY_RIGHT) {
      currX++;
      currDirection = RIGHT;
    }
    else if (command == 27) {
      endwin();
      break;
    } else {
      if (currDirection == UP) {
        currY--;
      } else if (currDirection == DOWN) {
        currY++;
      } else if (currDirection == LEFT) {
        currX--;
      } else if (currDirection == RIGHT) {
        currX++;
      }
      usleep(100000);
    }
    if (currY < 0) {
      currY = rows;
    } else if (currY > rows) {
      currY = 0;
    } else if (currX < 0) {
      currX = cols;
    } else if (currX > cols) {
      currX = 0;
    }

  }
  return 0;
}

//gcc main.c -lncursesw
