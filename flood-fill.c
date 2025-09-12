#include "core.h"
#include <string.h>

// TODO: use can_spread() here
static bool can_move() {
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

static void dfs(int row, int col, bool visited[GRID_ROWS][GRID_COLS], Color color, bool *touches_left, bool *touches_right) {
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
        if (col == 0) {
            *touches_left = true;
        }
        if (col == GRID_COLS - 1) {
            *touches_right = true;
        }
        dfs(row + 1, col, visited, color, touches_left, touches_right);
        dfs(row - 1, col, visited, color, touches_left, touches_right);
        dfs(row, col - 1, visited, color, touches_left, touches_right);
        dfs(row, col - 1, visited, color, touches_left, touches_right);
    }
}

static void remove_visited(bool visited[GRID_ROWS][GRID_COLS]) {
    for (int row = GRID_ROWS - 1; row >= 0; row--) {
        for (int col = 0; col < GRID_COLS; col++) {
            if (visited[row][col] == 1) {
                board[row][col] = (Pixel){0};
            }
        }
    }
}

void flood_fill() {
    if (can_move()) {
        return;
    }

    for (int row = GRID_ROWS - 1; row >= 0; row--) {
        // check from left most Pixel
        {
            bool touches_left = false;
            bool touches_right = false;
            Color color = board[row][0].color;
            bool visited[GRID_ROWS][GRID_COLS] = {0};
            dfs(row, 0, visited, color, &touches_left, &touches_right);

            if (touches_left && touches_right) {
                remove_visited(visited);
            }
        }

        // check from right most Pixel
        {
            bool touches_left = false;
            bool touches_right = false;
            bool visited[GRID_ROWS][GRID_COLS] = {0};
            Color color = board[row][GRID_COLS - 1].color;
            dfs(row, GRID_COLS - 1, visited, color, &touches_left, &touches_right);
            if (touches_left && touches_right) {
                remove_visited(visited);
            }
        }
    }
}

