#include <raylib.h>
#include "../core.h"

#define L_VERT_SEG_LEN 10
#define L_VERT_SEG_WIDTH 5
#define L_HORIZ_SEG_LEN 25
#define L_HORIZ_SEG_WIDTH 5

/*
 *           L_VERT_SEG_WIDTH
 *                  xx
 *  L_VERT_SEG_LEN  xx
 *                  xx
 *                  xxxxxxxxxxx
 *                  xxxxxxxxxxx   L_HORIZ_SEG_WIDTH
 *                L_HORIZ_SEG_LEN
 */

void create_L_block(GridPos pos, Color color) {
    for (int i = 0; i < L_VERT_SEG_LEN; i++) {     
        for (int j = 0; j < L_VERT_SEG_WIDTH; j++) {
            set_pixel((GridPos){pos.row + i, pos.col + j}, color);
        }
    }

    pos.row += L_VERT_SEG_LEN;

    for (int i = 0; i < L_HORIZ_SEG_WIDTH; i++) {
        for (int j = 0; j < L_HORIZ_SEG_LEN; j++) {
            set_pixel((GridPos){pos.row + i, pos.col + j}, color);
        }
    }
}

void move_L_block(GridPos src, GridPos dst) {
    
}
