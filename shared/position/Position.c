#include "Position.h"

void position_init(position_t* position, int xPos, int yPos) {
  position->xPos = xPos;
  position->yPos = yPos;
}

void position_update(position_t* position, int newX, int newY) {
  position->xPos = newX;
  position->yPos = newY;
}

void position_destroy(position_t* position) {
  position->xPos = -1;
  position->yPos = -1;
}
