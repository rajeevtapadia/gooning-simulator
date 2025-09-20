#include "raylib.h"
#include <stdbool.h>
#include <unistd.h>

#include "core.h"

Grid board = {0};
DragState drag_state = {0};
Figure options_panel[OPTIONS_COUNT];

bool is_game_over() {
    if (can_move()) {
        return false;
    }

    for (int i = 0; i < GRID_ROWS; i++) {
        if (board[CEILING_ROW][i].active) {
            drag_state.is_game_over = true;
            return true;
        }
    }
    return false;
}

int main(void) {
    const int pixel_width = PIXEL_SIZE;
    const int pixel_height = PIXEL_SIZE;
    const int window_width = SCREEN_COLS * pixel_width;
    const int window_height = SCREEN_ROWS * pixel_height;
    const int ceil_y = CEILING_ROW * PIXEL_SIZE;

    InitWindow(window_width, window_height, "gooning simulator");
    SetTargetFPS(80);

    while (!WindowShouldClose()) {
        generate_options();
        update_pixels();

        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawLine(0, ceil_y, window_width, ceil_y, YELLOW);
        if (is_game_over()) {
            DrawText("Game Over", window_width / 4, window_width / 2, 100, RED);
        }

        // render game grid
        for (int row = 0; row < GRID_ROWS; row++) {
            for (int col = 0; col < GRID_COLS; col++) {
                if (board[row][col].active) {
                    DrawRectangle(col * pixel_width, row * pixel_height, 5, 5, board[row][col].color);
                }
            }
        }

        DrawLine(0, GRID_ROWS * PIXEL_SIZE, window_width, GRID_ROWS * PIXEL_SIZE, GREEN);

        render_options_panel();

        if (drag_state.is_game_over) {
            EndDrawing();
            continue;
        }

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
