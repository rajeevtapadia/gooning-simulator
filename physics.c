#include "core.h"
#include <stdlib.h>

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

void process_pixel_movement(Grid board, Grid next_board, int row, int col) {
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
    Grid next_board = {0};

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
