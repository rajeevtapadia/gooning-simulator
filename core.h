#ifndef CORE_H
#define CORE_H

#include <raylib.h>
#include <sys/types.h>

#define GRID_ROWS 100
#define OPTIONS_ROWS 30
#define SCREEN_ROWS (GRID_ROWS + OPTIONS_ROWS)

#define SCREEN_COLS 100
#define GRID_COLS SCREEN_COLS
#define OPTIONS_COLS SCREEN_COLS

#define PIXEL_SIZE 10

typedef struct {
    Color color;
    bool active;
} Pixel;

typedef struct {
    int row;
    int col;
} GridPos;

typedef struct {
    bool dragging;
    int grid_row_offset;
    int grid_col_offset;
} DragState;

typedef Pixel Grid[GRID_ROWS][GRID_COLS];

typedef enum { LEFT, RIGHT, DOWN, NO_DIRECTION } Direction;

extern Grid board;

extern Pixel figure_options[(SCREEN_ROWS - GRID_ROWS)][GRID_COLS];

extern DragState drag_state;

Pixel *get_pixel(GridPos pos);

Pixel copy_pixel(GridPos pos);

void set_pixel(GridPos pos, Color color);

void reset_pixel(GridPos pos);

void move_pixel(GridPos from, GridPos to);

#endif // CORE_H

