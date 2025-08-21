#include "../core.h"
#include "block_shapes.h"

void create_square_block(GridPos pos, Color color) {
    int n = SQUARE_BLOCK_SIZE;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            board[pos.row + i][pos.col + j] = (Pixel){.color = color, .active = true};
        }
    }
}

void create_square_icon(GridPos pos, Color color) {
    int n = SQUARE_BLOCK_SIZE;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            figure_options[pos.row + i][pos.col + j] = (Pixel){.color = color, .active = true};
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

