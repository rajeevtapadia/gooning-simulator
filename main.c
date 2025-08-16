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
} Square;

typedef struct {
    int row;
    int col;
} GridPos;

void print_position(GridPos pos) { printf("row: %d col: %d\n", pos.row, pos.col); }

typedef enum {
    BLOCK_I, // Straight line
    BLOCK_O, // Square
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

Square board[GRID_ROWS][GRID_COLS] = {0};

Square *get_square(GridPos pos) { return &board[pos.row][pos.col]; }

Square copy_square(GridPos pos) {
    Square *square = get_square(pos);
    return (Square){square->color, square->active};
}

void set_square(GridPos pos, Color color) { *get_square(pos) = (Square){.color = color, .active = true}; }

void reset_square(GridPos pos) { *get_square(pos) = (Square){0}; }

void move_square(GridPos from, GridPos to) {
    Square *src = get_square(from);
    Square *dst = get_square(to);

    assert(src->active == true && "No square found");
    assert(dst->active == false && "Position already occupied");

    dst->active = true;
    dst->color = src->color;
    reset_square(from);
}

void create_square_block(GridPos pos, Color color) {
    int n = SQUARE_BLOCK_SIZE;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            board[pos.row + i][pos.col + j] = (Square){.color = color, .active = true};
        }
    }
}

void move_square_block(GridPos src, GridPos dst) {
    int n = SQUARE_BLOCK_SIZE;
    Square sample = copy_square(src);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            reset_square((GridPos){.row = src.row + i, .col = src.col + j});
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            set_square((GridPos){.row = dst.row + i, .col = dst.col + j}, sample.color);
        }
    }
}

int main(void) {
    printf("gooning simulator\n");
    const int square_width = 10;
    const int square_height = 10;
    const int window_width = GRID_ROWS * square_width;
    const int window_height = GRID_COLS * square_height;

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
            // move_square(pos, new_pos);
            move_square_block(pos, new_pos);
            pos = new_pos;
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);
        for (int row = 0; row < GRID_ROWS; row++) {
            for (int col = 0; col < GRID_COLS; col++) {
                if (board[row][col].active) {
                    DrawRectangle(col * square_width, row * square_height, 5, 5, board[row][col].color);
                }
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

