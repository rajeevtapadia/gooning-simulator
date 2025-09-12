#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "block-shapes/block_shapes.h"
#include "core.h"

Grid board = {0};
DragState drag_state = {0};
Figure options_panel[OPTIONS_COUNT];

int main(void) {
    printf("gooning simulator\n");
    const int pixel_width = PIXEL_SIZE;
    const int pixel_height = PIXEL_SIZE;
    const int window_width = SCREEN_COLS * pixel_width;
    const int window_height = SCREEN_ROWS * pixel_height;

    InitWindow(window_width, window_height, "gooning simulator");
    SetTargetFPS(60);

    GridPos square_pos = {.col = GRID_COLS / 2, .row = 3 * GRID_ROWS / 4};

    create_square_block(square_pos, GREEN);
    // create_L_block(square_pos, GREEN);

    float timer = 0.0f;
    float delay = 0.005f;

    while (!WindowShouldClose()) {
        generate_options();

        float dt = GetFrameTime();
        timer += dt;

        if (timer >= delay && square_pos.row < GRID_ROWS) {
            timer = 0;
            GridPos new_square_pos = {.row = square_pos.row + 1, .col = square_pos.col};
            square_pos = new_square_pos;
        }
        update_pixels();

        BeginDrawing();
        ClearBackground(DARKGRAY);
        // render game grid
        for (int row = 0; row < GRID_ROWS; row++) {
            for (int col = 0; col < GRID_COLS; col++) {
                if (board[row][col].active) {
                    DrawRectangle(col * pixel_width, row * pixel_height, 5, 5, board[row][col].color);
                }
            }
        }

        DrawLine(0, GRID_ROWS * PIXEL_SIZE, window_width, GRID_ROWS * PIXEL_SIZE, GREEN);

        // render figure options below game grid
        render_options_panel();

        detect_dragging();
        perform_dragging();

        if (!drag_state.dragging) {
            flood_fill();
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

