#include "core.h"
#include <stdio.h>
#include <string.h>

void dump_board() {
    printf("  ");
    for (int i = 0; i < GRID_COLS; i++) {
        printf("%2d", i);
    }
    printf("\n");
    for (int row = 0; row < GRID_ROWS; row++) {
        printf("%2d ", row);
        for (int col = 0; col < GRID_COLS; col++) {
            printf("%d ", board[row][col].active);
        }
        printf("\n");
    }
}

Pixel *get_pixel(GridPos pos) {
    return &board[pos.row][pos.col];
}

// TODO: change to copy_pixel(dst, src)
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

// Should this be move_pixel(dst, src) for sake of consistency ??
void move_pixel(GridPos from, GridPos to) {
    Pixel *src = get_pixel(from);
    Pixel *dst = get_pixel(to);

    assert(src->active == true && "No pixel found");
    assert(dst->active == false && "Position already occupied");

    dst->active = true;
    dst->color = src->color;
    reset_pixel(from);
}

void copy_board(Grid dst, Grid src) {
    memcpy(dst, src, GRID_ROWS * GRID_COLS * sizeof(Pixel));
}

// returns
//      true if block is free
//      false if block is not free
bool is_block_empty(int row, int col) {
    assert(row > 0 && col > 0 && "out of bounds");
    assert(row < GRID_ROWS && col < GRID_COLS && "out of bounds");
    if (row < 0 || col < 0 || row >= GRID_ROWS || col >= GRID_COLS) {
        return false;
    }
    return !board[row][col].active;
}

