#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "core.h"

Grid board = {0};
DragState drag_state = {0};
Figure options_panel[OPTIONS_COUNT];
Score game_score;

Color BG_COLOR = (Color){30, 30, 30, 255};
Color LINE_COLOR = (Color){200, 200, 200, 255};

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

size_t calculate_score() {
    if(game_score.max_gooning_streak < game_score.curr_gooning_streak) {
        game_score.max_gooning_streak = game_score.curr_gooning_streak;
    }
    return game_score.pixels_destroyed * 0.1;
}

void write_score_to_screen() {
#ifdef SFW
    const char *score_prompt = "Score";
    const char *streak_prompt = "Streak";
#else
    const char *score_prompt = "Gooning Score";
    const char *streak_prompt = "Gooning Streak";
#endif

    char score_str[36];
    sprintf(score_str, "%s: %zu", score_prompt, calculate_score());
    DrawText(score_str, 0, 0, 30, PURPLE);

    char streak_str[37];
    sprintf(streak_str, "%s: %zu", streak_prompt, game_score.max_gooning_streak);
    DrawText(streak_str, 0, 35, 30, PURPLE);
}

int main(void) {
    srand(time(0));
    const int pixel_width = PIXEL_SIZE;
    const int pixel_height = PIXEL_SIZE;
    const int window_width = SCREEN_COLS * pixel_width;
    const int window_height = SCREEN_ROWS * pixel_height;
    const int ceil_y = CEILING_ROW * PIXEL_SIZE;

#ifdef SFW
    const char *window_title = "stupid game";
#else
    const char *window_title = "gooning simulator";
#endif

    InitWindow(window_width, window_height, window_title);
    SetTargetFPS(80);

    while (!WindowShouldClose()) {
        generate_options();
        update_pixels();

        BeginDrawing();
        ClearBackground(BG_COLOR);
        DrawLine(0, ceil_y, window_width, ceil_y, LINE_COLOR);

        // render game grid
        for (int row = 0; row < GRID_ROWS; row++) {
            for (int col = 0; col < GRID_COLS; col++) {
                if (board[row][col].active) {
                    DrawRectangle(col * pixel_width, row * pixel_height, 5, 5, board[row][col].color);
                }
            }
        }

        DrawLine(0, GRID_ROWS * PIXEL_SIZE, window_width, GRID_ROWS * PIXEL_SIZE, LINE_COLOR);

        render_options_panel();

        if (is_game_over()) {
            DrawText("Game Over", 133, 400, 100, RED);
        }

        if (drag_state.is_game_over) {
            EndDrawing();
            continue;
        }

        detect_dragging();
        perform_dragging();

        if (!drag_state.dragging) {
            size_t rm_count = flood_fill();
            game_score.pixels_destroyed += rm_count;
        }

        write_score_to_screen();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
