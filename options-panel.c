#include "core.h"
#include <stdlib.h>

static bool is_panel_empty() {
    for (int i = 0; i < OPTIONS_COUNT; i++) {
        if (options_panel[i].active) {
            return false;
        }
    }
    return true;
}

static BlockType get_random_block_type() {
    return (BlockType)(rand() % BLOCK_TYPE_COUNT);
}

static void draw_square_on_board(Figure *option, int row, int col, int size) {
    for (int i = row; i < row + size; i++) {
        for (int j = col; j < col + size; j++) {
            option->mask[i][j] = true;
        }
    }
}

static void generate_mask(Figure *option) {
    int step, size;
    switch (option->type) {
    case BLOCK_SQUARE:
        for (int row = 0; row < SQUARE_BLOCK_SIZE; row++) {
            for (int col = 0; col < SQUARE_BLOCK_SIZE; col++) {
                option->mask[row][col] = true;
            }
        }
        break;
    case BLOCK_L:
        for (int row = 0; row < BLOCK_L_HEIGHT / 2; row++) {
            for (int col = 0; col < BLOCK_L_HEIGHT / 2; col++) {
                option->mask[row][col] = true;
            }
        }
        for (int row = BLOCK_L_HEIGHT / 2; row < BLOCK_L_HEIGHT; row++) {
            for (int col = 0; col < BLOCK_L_WIDTH; col++) {
                option->mask[row][col] = true;
            }
        }
        break;
    case BLOCK_J:
        step = BLOCK_SCALING_FACTOR;
        size = BLOCK_SCALING_FACTOR;
        draw_square_on_board(option, 0 * step, 2 * step, size);
        draw_square_on_board(option, 1 * step, 0 * step, size);
        draw_square_on_board(option, 1 * step, 1 * step, size);
        draw_square_on_board(option, 1 * step, 2 * step, size);
        break;
    case BLOCK_Z:
        step = BLOCK_SCALING_FACTOR;
        size = BLOCK_SCALING_FACTOR;
        draw_square_on_board(option, 0 * step, 0 * step, size);
        draw_square_on_board(option, 0 * step, 1 * step, size);
        draw_square_on_board(option, 1 * step, 1 * step, size);
        draw_square_on_board(option, 1 * step, 2 * step, size);
        break;
    case BLOCK_S:
        step = BLOCK_SCALING_FACTOR;
        size = BLOCK_SCALING_FACTOR;
        draw_square_on_board(option, 0 * step, 2 * step, size);
        draw_square_on_board(option, 0 * step, 1 * step, size);
        draw_square_on_board(option, 1 * step, 1 * step, size);
        draw_square_on_board(option, 1 * step, 0 * step, size);
        break;
    case BLOCK_T:
        step = BLOCK_SCALING_FACTOR;
        size = BLOCK_SCALING_FACTOR;
        draw_square_on_board(option, 0 * step, 1 * step, size);
        draw_square_on_board(option, 1 * step, 0 * step, size);
        draw_square_on_board(option, 1 * step, 1 * step, size);
        draw_square_on_board(option, 1 * step, 2 * step, size);
        break;
    case BLOCK_I:
        step = BLOCK_SCALING_FACTOR;
        size = BLOCK_SCALING_FACTOR;
        draw_square_on_board(option, 0, 0 * step, size);
        draw_square_on_board(option, 0, 1 * step, size);
        draw_square_on_board(option, 0, 2 * step, size);
        draw_square_on_board(option, 0, 3 * step, size);
        break;
    default:
        assert("UNIMPLEMENTED");
    }
}

static void set_fig_width_and_height(Figure *fig) {
    switch (fig->type) {
    case BLOCK_SQUARE:
        fig->width = SQUARE_BLOCK_SIZE;
        fig->height = SQUARE_BLOCK_SIZE;
        break;
    case BLOCK_L:
        fig->width = BLOCK_L_WIDTH;
        fig->height = BLOCK_L_HEIGHT;
        break;
    case BLOCK_J:
        fig->width = BLOCK_J_WIDTH;
        fig->height = BLOCK_J_HEIGHT;
        break;
    case BLOCK_Z:
        fig->width = BLOCK_Z_WIDTH;
        fig->height = BLOCK_Z_HEIGHT;
        break;
    case BLOCK_S:
        fig->width = BLOCK_S_WIDTH;
        fig->height = BLOCK_S_HEIGHT;
        break;
    case BLOCK_T:
        fig->width = BLOCK_T_WIDTH;
        fig->height = BLOCK_T_HEIGHT;
        break;
    case BLOCK_I:
        fig->width = BLOCK_I_WIDTH;
        fig->height = BLOCK_I_HEIGHT;
        break;
    default:
        assert("invalid shape passed in set_fig_width_and_height()");
    }
}

Color get_random_color() {
    Color C_BLUE = (Color){52, 152, 219, 255};  // Blue
    Color C_RED = (Color){231, 76, 60, 255};    // Red
    Color C_GREEN = (Color){46, 204, 113, 255}; // Green
    Color colors[] = {C_BLUE, C_RED, C_GREEN};
    // Color colors[] = {BLUE, RED, GREEN};
    int n = sizeof(colors) / sizeof(colors[0]);
    return colors[rand() % n];
}

//
// TODO: select figures and colors by random choice
void generate_options() {
    if (!is_panel_empty() || drag_state.dragging)
        return;

    int options_panel_col_width = OPTIONS_COLS / OPTIONS_COUNT;

    for (int i = 0; i < OPTIONS_COUNT; i++) {
        options_panel[i] = (Figure){
            .color = get_random_color(),
            .type = get_random_block_type(),
            .pos = (GridPos){.row = GRID_ROWS + 1, .col = i * options_panel_col_width},
            .active = true,
        };

        set_fig_width_and_height(&options_panel[i]);
        generate_mask(&options_panel[i]);
    }
}

void render_options_panel() {
    int slot_width = SCREEN_COLS / OPTIONS_COUNT;
    for (int i = 0; i < OPTIONS_COUNT; i++) {
        Figure option_fig = options_panel[i];

        // center the figure in its slot
        int fig_col_start = (slot_width / 2) - (option_fig.width / 2);

        for (int i = 0; i < MAX_FIGURE_ROW; i++) {
            for (int j = 0; j < MAX_FIGURE_COL; j++) {
                if (option_fig.mask[i][j]) {
                    int x = (option_fig.pos.col + j + fig_col_start) * PIXEL_SIZE;
                    int y = (option_fig.pos.row + i) * PIXEL_SIZE;
                    DrawRectangle(x, y, 5, 5, option_fig.color);
                }
            }
        }
    }
}
