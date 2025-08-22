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
#define OPTIONS_COUNT 3

typedef enum {
    BLOCK_I, // Straight line
    BLOCK_SQUARE, // Square
    BLOCK_T, // T-shape
    BLOCK_S, // S-shape
    BLOCK_Z, // Z-shape
    BLOCK_J, // J-shape (L mirrored)
    BLOCK_L  // L-shape
} BlockType;

typedef enum { LEFT, RIGHT, DOWN, NO_DIRECTION } Direction;

typedef struct {
    Color color;
    bool active;
} Pixel;

typedef struct {
    int row;
    int col;
} GridPos;

typedef struct {
    Color color;
    BlockType type;
    GridPos pos;
    int total_row;
    int total_col;
    bool active;
} Figure;

typedef struct {
    bool dragging;
    int grid_row_offset;
    int grid_col_offset;
} DragState;

typedef Pixel Grid[GRID_ROWS][GRID_COLS];

extern Grid board;

extern DragState drag_state;

Pixel *get_pixel(GridPos pos);

Pixel copy_pixel(GridPos pos);

void set_pixel(GridPos pos, Color color);

void reset_pixel(GridPos pos);

void move_pixel(GridPos from, GridPos to);

#endif // CORE_H

