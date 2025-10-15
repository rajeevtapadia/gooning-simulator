#include "core.h"
#include <string.h>

// TODO: use can_spread() here
bool can_move() {
    for (int row = GRID_ROWS - 2; row >= 0; row--) {
        for (int col = 1; col < GRID_COLS - 1; col++) {
            if (board[row][col].active) {

                if (!board[row + 1][col].active)
                    return true;
                if (!board[row + 1][col - 1].active || !board[row + 1][col + 1].active)
                    return true;
            }
        }
    }
    return false;
}

static void dfs(int row, int col, bool visited[GRID_ROWS][GRID_COLS], Color color, bool *touches_right) {
    if (row < 0 || row >= GRID_ROWS) {
        return;
    }
    if (col < 0 || col >= GRID_COLS) {
        return;
    }
    if (!board[row][col].active) {
        return;
    }
    if (visited[row][col]) {
        return;
    }
    if (memcmp(&board[row][col].color, &color, sizeof(color)) == 0) {
        visited[row][col] = true;
        if (col == GRID_COLS - 1) {
            *touches_right = true;
        }
        // sides
        dfs(row + 1, col, visited, color, touches_right);
        dfs(row - 1, col, visited, color, touches_right);
        dfs(row, col - 1, visited, color, touches_right);
        dfs(row, col - 1, visited, color, touches_right);

        // diagonals
        dfs(row + 1, col + 1, visited, color, touches_right);
        dfs(row - 1, col + 1, visited, color, touches_right);
        dfs(row + 1, col - 1, visited, color, touches_right);
        dfs(row - 1, col - 1, visited, color, touches_right);
    }
}

static size_t remove_visited(bool visited[GRID_ROWS][GRID_COLS]) {
    size_t rm_count = 0;
    for (int row = GRID_ROWS - 1; row >= 0; row--) {
        for (int col = 0; col < GRID_COLS; col++) {
            if (visited[row][col] == 1) {
                board[row][col] = (Pixel){0};
                rm_count++;
            }
        }
    }
    return rm_count;
}

size_t flood_fill() {
    if (can_move()) {
        return 0;
    }

    for (int row = GRID_ROWS - 1; row >= 0; row--) {
        // check from left most Pixel
        {
            bool touches_right = false;
            Color color = board[row][0].color;
            bool visited[GRID_ROWS][GRID_COLS] = {0};
            dfs(row, 0, visited, color, &touches_right);

            if (touches_right) {
                game_score.curr_gooning_streak++;
                return remove_visited(visited);
            }
        }
    }
    return 0;
}
