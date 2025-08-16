#ifndef BLOCK_SHAPES_H
#define BLOCK_SHAPES_H

#include <raylib.h>
#include "../core.h"

// square shape
#define SQUARE_BLOCK_SIZE 10

void create_square_block(GridPos pos, Color color);
void move_square_block(GridPos src, GridPos dst);


// L shape
void create_L_block(GridPos pos, Color color);
void move_L_block(GridPos src, GridPos dst);

#endif // BLOCK_SHAPES_H

