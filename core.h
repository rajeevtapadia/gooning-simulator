#ifndef CORE_H
#define CORE_H

#include <assert.h>
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

#define MAX_FIGURE_ROW 40
#define MAX_FIGURE_COL 32

#define SPREAD_CHANCE 50

#define CEILING_ROW 15

typedef enum {
    BLOCK_I,      // Straight line
    BLOCK_SQUARE, // Square
    BLOCK_T,      // T-shape
    BLOCK_S,      // S-shape
    BLOCK_Z,      // Z-shape
    BLOCK_J,      // J-shape (L mirrored)
    BLOCK_L,      // L-shape
    BLOCK_TYPE_COUNT
} BlockType;

// BlockType dimension constants
#define BLOCK_SCALING_FACTOR 8
#define SQUARE_BLOCK_SIZE 16
#define BLOCK_L_WIDTH 24
#define BLOCK_L_HEIGHT 14
#define BLOCK_J_WIDTH 3 * BLOCK_SCALING_FACTOR
#define BLOCK_J_HEIGHT 2 * BLOCK_SCALING_FACTOR
#define BLOCK_S_WIDTH 3 * BLOCK_SCALING_FACTOR
#define BLOCK_S_HEIGHT 2 * BLOCK_SCALING_FACTOR
#define BLOCK_Z_WIDTH 3 * BLOCK_SCALING_FACTOR
#define BLOCK_Z_HEIGHT 2 * BLOCK_SCALING_FACTOR
#define BLOCK_T_WIDTH 3 * BLOCK_SCALING_FACTOR
#define BLOCK_T_HEIGHT 2 * BLOCK_SCALING_FACTOR
#define BLOCK_I_WIDTH 4 * BLOCK_SCALING_FACTOR
#define BLOCK_I_HEIGHT 1 * BLOCK_SCALING_FACTOR

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
    bool mask[MAX_FIGURE_ROW][MAX_FIGURE_COL];
    GridPos pos;
    int width;
    int height;
    bool active;
} Figure;

typedef struct {
    bool dragging;
    GridPos position_offset;
    Figure ghost_figure;
    int fig_idx;
    bool is_game_over;
} DragState;

typedef Pixel Grid[GRID_ROWS][GRID_COLS];

extern Grid board;
extern DragState drag_state;
extern Figure options_panel[OPTIONS_COUNT];

// physics.c
void update_pixels();
Direction can_spread(int row, int col);

// options-panel.c
void generate_options();
void render_options_panel();

// drag-and-drop.c
void detect_dragging();
void perform_dragging();

// flood-fill.c
void flood_fill();
bool can_move();

// game-grid.c
Pixel copy_pixel(GridPos pos);
void copy_board(Grid dst, Grid src);
bool is_block_empty(int row, int col);
void set_pixel(GridPos pos, Color color);
void reset_pixel(GridPos pos);

// util.c
void print_position(GridPos pos);
GridPos vector2_to_grid_pos(Vector2 vector);
Vector2 grid_pos_to_vector2(GridPos pos);

#endif // CORE_H
