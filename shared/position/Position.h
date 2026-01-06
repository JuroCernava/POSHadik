#ifndef POSITION_H
#define POSITION_H

typedef struct {
  int xPos;
  int yPos;
} position_t;

void position_init(position_t* position, int xPos, int yPos);
void position_update(position_t* position, int newX, int newY);
void position_destroy(position_t* position);


#endif
