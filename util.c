#include "core.h"
#include <stdio.h>

void print_position(GridPos pos) {
    printf("row: %d col: %d\n", pos.row, pos.col);
}


GridPos vector2_to_grid_pos(Vector2 vector) {
    return (GridPos){.row = vector.y / PIXEL_SIZE, .col = vector.x / PIXEL_SIZE};
}

Vector2 grid_pos_to_vector2(GridPos pos) {
    return (Vector2){.x = pos.col * PIXEL_SIZE, .y = pos.row * PIXEL_SIZE};
}

