#include "raylib.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define GRID_ROWS 100
#define GRID_COLS 100
#define SQUARE_BLOCK_SIZE 10

typedef struct {
    Color color;
    bool active;
} Pixel;

typedef struct {
    int row;
    int col;
} GridPos;

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

void create_square_block(GridPos pos, Color color) {
    int n = SQUARE_BLOCK_SIZE;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            board[pos.row + i][pos.col + j] = (Pixel){.color = color, .active = true};
        }
    }
}

void move_square_block(GridPos src, GridPos dst) {
    int n = SQUARE_BLOCK_SIZE;
    Pixel sample = copy_pixel(src);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            reset_pixel((GridPos){.row = src.row + i, .col = src.col + j});
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            set_pixel((GridPos){.row = dst.row + i, .col = dst.col + j}, sample.color);
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

    create_square_block(pos, GREEN);

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
            move_square_block(pos, new_pos);
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
