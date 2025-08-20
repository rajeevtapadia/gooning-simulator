#ifndef CORE_H
#define CORE_H

#include <raylib.h>

#define GRID_ROWS 100
#define GRID_COLS 100

typedef struct {
    Color color;
    bool active;
} Pixel;

typedef struct {
    int row;
    int col;
} GridPos;

typedef Pixel Grid[GRID_ROWS][GRID_COLS];

typedef enum { LEFT, RIGHT, DOWN, NO_DIRECTION } Direction;

extern Grid board;

Pixel *get_pixel(GridPos pos);

Pixel copy_pixel(GridPos pos);

void set_pixel(GridPos pos, Color color);

void reset_pixel(GridPos pos);

void move_pixel(GridPos from, GridPos to);

#endif // CORE_H

