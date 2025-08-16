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
    return !board[row][col].active;
}

void gravity_is_a_bitch() {
    for (int row = GRID_ROWS - 2; row >= 0; row--) {
        for (int col = 0; col < GRID_COLS; col++) {
            if(board[row][col].active && is_block_empty(row+1, col)) {
                move_pixel((GridPos){row, col}, (GridPos){row+1, col});
            }
        }
    }
}

int main(void) {
    printf("gooning simulator\n");
    const int pixel_width = 10;
    const int pixel_height = 10;
    const int window_width = GRID_ROWS * pixel_width;
    const int window_height = GRID_COLS * pixel_height;

    InitWindow(window_width, window_height, "gooning simulator");
    SetTargetFPS(60);

    GridPos pos = {.col = GRID_COLS / 2, .row = GRID_ROWS / 2};

    // create_square_block(pos, GREEN);
    create_L_block(pos, GREEN);

    float timer = 0.0f;
    float delay = 0.2f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        timer += dt;

        if (timer >= delay && pos.row < GRID_ROWS - SQUARE_BLOCK_SIZE) {
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
