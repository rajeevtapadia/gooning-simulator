#include "raylib.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "block-shapes/block_shapes.h"
#include "core.h"

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

typedef struct {
    Vector2 *square_list;
    BlockType block_type;
} Block;

Pixel board[GRID_ROWS][GRID_COLS] = {0};

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

void move_pixel(GridPos from, GridPos to) {
    Pixel *src = get_pixel(from);
    Pixel *dst = get_pixel(to);

    assert(src->active == true && "No pixel found");
    assert(dst->active == false && "Position already occupied");

    dst->active = true;
    dst->color = src->color;
    reset_pixel(from);
}

bool is_block_empty(int row, int col) {
    // if (row < 0 || col < 0 || row >= GRID_ROWS || col >= GRID_COLS) {
    //     return false;
    // }
    return !board[row][col].active;
}

/*
 * returns
 *      -1 if pixel can fall on left
 *       1 if pixel can fall on right
 *       0 if pixel can't move
 */
int can_spread(int row, int col) {
    if (row >= GRID_ROWS - 1)
        return 0;

    if (is_block_empty(row + 1, col))
        return 0;

    if (col > 0 && is_block_empty(row + 1, col - 1)) {
        return -1;
    }
    if (col < GRID_COLS - 1 && is_block_empty(row + 1, col + 1)) {
        return 1;
    }
    return 0;
}

void gravity_is_a_bitch() {
    for (int row = GRID_ROWS - 2; row >= 0; row--) {
        for (int col = 0; col < GRID_COLS; col++) {
            if(is_block_empty(row, col)) continue;

            // straight fall
            if (row < GRID_ROWS -1 && is_block_empty(row + 1, col)) {
                // printf("falling\n");
                // printf("row %d col %d dir %d\n", row, col, 0);
                move_pixel((GridPos){row, col}, (GridPos){row + 1, col});
                continue;
            } 

            // spread
            // printf("spreading\n");
            int dir = can_spread(row, col);
            // printf("row %d col %d dir %d\n", row, col, dir);
            // dump_board();
            switch (dir) {
            case -1:
                move_pixel((GridPos){row, col}, (GridPos){row + 1, col - 1});
                break;
            case 1:
                move_pixel((GridPos){row, col}, (GridPos){row + 1, col + 1});
                break;
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
    SetTargetFPS(10);

    GridPos pos = {.col = GRID_COLS / 2, .row = 3 * GRID_ROWS / 4};

    create_square_block(pos, GREEN);
    // create_L_block(pos, GREEN);

    float timer = 0.0f;
    float delay = 0.2f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        timer += dt;

        if (timer >= delay) {
            timer = 0;
            print_position(pos);
            GridPos new_pos = {.row = pos.row + 1, .col = pos.col};
            // move_pixel(pos, new_pos);
            // move_square_block(pos, new_pos);
            gravity_is_a_bitch();
            pos = new_pos;
        }

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
