#include "raylib.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "block-shapes/block_shapes.h"
#include "core.h"

#define SPREAD_CHANCE 50

void print_position(GridPos pos) {
    printf("row: %d col: %d\n", pos.row, pos.col);
}

typedef enum {
    BLOCK_I, // Straight line
    BLOCK_O, // Pixel
    BLOCK_T, // T-shape
    BLOCK_S, // S-shape
    BLOCK_Z, // Z-shape
    BLOCK_J, // J-shape (L mirrored)
    BLOCK_L  // L-shape
} BlockType;

Pixel board[GRID_ROWS][GRID_COLS] = {0};

void copy_board(Pixel dst[GRID_ROWS][GRID_COLS], Pixel src[GRID_ROWS][GRID_COLS]) {
    memcpy(dst, src, GRID_ROWS * GRID_COLS * sizeof(Pixel));
}

void dump_board() {
    printf("  ");
    for (int i = 0; i < GRID_COLS; i++) {
        printf("%2d", i);
    }
    printf("\n");
    for (int row = 0; row < GRID_ROWS; row++) {
        printf("%2d ", row);
        for (int col = 0; col < GRID_COLS; col++) {
            printf("%d ", board[row][col].active);
        }
        printf("\n");
    }
}

Pixel *get_pixel(GridPos pos) {
    return &board[pos.row][pos.col];
}

// TODO: change to copy_pixel(dst, src)
Pixel copy_pixel(GridPos pos) {
    Pixel *pixel = get_pixel(pos);
    return (Pixel){pixel->color, pixel->active};
}

void set_pixel(GridPos pos, Color color) {
    *get_pixel(pos) = (Pixel){.color = color, .active = true};
}

void reset_pixel(GridPos pos) {
    *get_pixel(pos) = (Pixel){0};
}

// Should this be move_pixel(dst, src) for sake of consistency ??
void move_pixel(GridPos from, GridPos to) {
    Pixel *src = get_pixel(from);
    Pixel *dst = get_pixel(to);

    assert(src->active == true && "No pixel found");
    assert(dst->active == false && "Position already occupied");

    dst->active = true;
    dst->color = src->color;
    reset_pixel(from);
}

// returns
//      true if block is free
//      false if block is not free
bool is_block_empty(int row, int col) {
    assert(row > 0 && col > 0 && "out of bounds");
    assert(row < GRID_ROWS && col < GRID_COLS && "out of bounds");
    if (row < 0 || col < 0 || row >= GRID_ROWS || col >= GRID_COLS) {
        return false;
    }
    return !board[row][col].active;
}

bool can_spread_left(int row, int col) {
    if (col > 0 && is_block_empty(row + 1, col - 1)) {
        return true;
    }
    return false;
}
bool can_spread_right(int row, int col) {
    if (col < GRID_COLS - 1 && is_block_empty(row + 1, col + 1)) {
        return true;
    }
    return false;
}

Direction can_spread(int row, int col) {
    assert(row < GRID_ROWS && "row out of bounds");
    assert(col < GRID_COLS && "col out of bounds");

    if (is_block_empty(row + 1, col))
        return DOWN;

    bool left = can_spread_left(row, col);
    bool right = can_spread_right(row, col);

    if (left && right) {
        bool side = (rand() % 2 == 0);
        return (side ? -1 : 1);
    } else if (left) {
        return LEFT;
    } else if (right) {
        return RIGHT;
    }

    return NO_DIRECTION;
}

void process_pixel_movement(Pixel board[GRID_ROWS][GRID_COLS], Pixel next_board[GRID_ROWS][GRID_COLS], int row, int col) {
    if (!board[row][col].active) {
        return;
    }

    // Pixels in last row cannot move anywhere
    if (row == GRID_ROWS - 1) {
        next_board[row][col] = board[row][col];
        return;
    }

    // check for possible fall directions
    if (!next_board[row + 1][col].active) {
        next_board[row + 1][col] = board[row][col];
    } else {
        bool can_go_left = (col > 0) && !next_board[row + 1][col - 1].active;
        bool can_go_right = (col < GRID_COLS - 1) && !next_board[row + 1][col + 1].active;

        /*
         * To get natural and uniform spreading, a pixel will fall
         * diagonally (spread) if two conditions are met:
         *
         * 1. An empty space exists one row down and to the left OR right.
         * 2. A random probability check passes, controlled by SPREAD_CHANCE.
         */
        if ((can_go_left || can_go_right) && (rand() % 100 < SPREAD_CHANCE)) {
            if (can_go_left && can_go_right) {
                if (rand() % 2 == 0) {
                    next_board[row + 1][col - 1] = board[row][col];
                } else {
                    next_board[row + 1][col + 1] = board[row][col];
                }
            } else if (can_go_left) {
                next_board[row + 1][col - 1] = board[row][col];
            } else {
                next_board[row + 1][col + 1] = board[row][col];
            }

        } else {
            // No possible movement
            next_board[row][col] = board[row][col];
        }
    }
}

void update_pixels() {
    static bool col_scan_direction = true;
    Pixel next_board[GRID_ROWS][GRID_COLS] = {0};

    for (int row = GRID_ROWS - 1; row >= 0; row--) {
        if (col_scan_direction) {
            for (int col = 0; col < GRID_COLS; col++) {
                process_pixel_movement(board, next_board, row, col);
            }
        } else {
            for (int col = GRID_COLS - 1; col >= 0; col--) {
                process_pixel_movement(board, next_board, row, col);
            }
        }
    }

    col_scan_direction = !col_scan_direction;
    copy_board(board, next_board);
}

void gravity_is_a_bitch() {
    for (int row = GRID_ROWS - 2; row >= 0; row--) {
        for (int col = 0; col < GRID_COLS; col++) {
            if (is_block_empty(row, col))
                continue;

            // straight fall
            if (is_block_empty(row + 1, col)) {
                // printf("falling\n");
                // printf("row %d col %d dir %d\n", row, col, 0);
                move_pixel((GridPos){row, col}, (GridPos){row + 1, col});
                continue;
            }
        }
    }
    printf("done\n");
}

int main(void) {
    printf("gooning simulator\n");
    const int pixel_width = 10;
    const int pixel_height = 10;
    const int window_width = GRID_ROWS * pixel_width;
    const int window_height = GRID_COLS * pixel_height;

    InitWindow(window_width, window_height, "gooning simulator");
    SetTargetFPS(30);

    GridPos pos = {.col = GRID_COLS / 2, .row = 3 * GRID_ROWS / 4};

    create_square_block(pos, GREEN);
    // create_L_block(pos, GREEN);

    float timer = 0.0f;
    float delay = 0.005f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        timer += dt;

        if (timer >= delay && pos.row < GRID_ROWS) {
            timer = 0;
            print_position(pos);
            GridPos new_pos = {.row = pos.row + 1, .col = pos.col};
            // move_pixel(pos, new_pos);
            // move_square_block(pos, new_pos);
            // gravity_is_a_bitch();
            // spread_those_pixels();
            pos = new_pos;
        }
        update_pixels();

        BeginDrawing();
        ClearBackground(DARKGRAY);
        for (int row = 0; row < GRID_ROWS; row++) {
            for (int col = 0; col < GRID_COLS; col++) {
                if (board[row][col].active) {
                    DrawRectangle(col * pixel_width, row * pixel_height, 5, 5, board[row][col].color);
                }
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
