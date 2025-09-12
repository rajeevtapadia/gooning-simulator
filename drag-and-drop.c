#include "core.h"
#include <stdio.h>

static int is_on_which_square(GridPos mouse_pos) {
    for (int i = 0; i < OPTIONS_COUNT; i++) {
        Figure fig = options_panel[i];
        bool is_overlapping = (mouse_pos.row >= fig.pos.row) && (mouse_pos.row < fig.pos.row + fig.height) &&
                              (mouse_pos.col >= fig.pos.col) && (mouse_pos.col < fig.pos.col + fig.width);
        if (is_overlapping) {
            return i;
        }
    }
    return -1;
}

void detect_dragging() {
    if (drag_state.dragging) {
        return;
    }
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return;
    }
    Vector2 mouse = GetMousePosition();
    GridPos mouse_pos = vector2_to_grid_pos(mouse);
    print_position(mouse_pos);
    int fig_idx = is_on_which_square(mouse_pos);
    printf("dragging fig: %d\n", fig_idx);
    if (fig_idx != -1) {
        drag_state = (DragState){
            .dragging = true,
            .position_offset = mouse_pos,
            .ghost_figure = options_panel[fig_idx],
            .fig_idx = fig_idx,
        };
        options_panel[fig_idx] = (Figure){0};
    }
}

static void draw_ghost(Vector2 mouse) {
    Figure fig = drag_state.ghost_figure;
    for (int i = 0; i < fig.height; i++) {
        for (int j = 0; j < fig.width; j++) {
            if (fig.mask[i][j]) {
                int x = mouse.x + (j * PIXEL_SIZE);
                int y = mouse.y + (i * PIXEL_SIZE);
                DrawRectangle(x, y, 5, 5, fig.color);
            }
        }
    }
}

static bool collides_with_game_grid(GridPos mouse_pos) {
    int fig_width = drag_state.ghost_figure.width;
    int fig_height = drag_state.ghost_figure.height;
    for (int i = 0; i < fig_height; i++) {
        for (int j = 0; j < fig_width; j++) {
            int row = mouse_pos.row + i;
            int col = mouse_pos.col + j;
            if (row < GRID_ROWS && col < GRID_COLS && board[row][col].active) {
                return true;
            }
        }
    }
    return false;
}

static void handle_drop(GridPos mouse_pos) {
    if (mouse_pos.row < GRID_ROWS && !collides_with_game_grid(mouse_pos)) {
        Figure fig = drag_state.ghost_figure;
        for (int i = 0; i < MAX_FIGURE_ROW; i++) {
            for (int j = 0; j < MAX_FIGURE_COL; j++) {
                if (fig.mask[i][j]) {
                    int row = mouse_pos.row + i;
                    int col = mouse_pos.col + j;
                    board[row][col] = (Pixel){.color = fig.color, .active = true};
                }
            }
        }
    } else {
        options_panel[drag_state.fig_idx] = drag_state.ghost_figure;
        drag_state = (DragState){0};
    }
    drag_state = (DragState){0};
}

static bool validate_mouse_pos(GridPos mouse_pos) {
    int row = mouse_pos.row;
    int col = mouse_pos.col;
    if (row >= 0 && row < SCREEN_ROWS && col >= 0 && col <= SCREEN_COLS - drag_state.ghost_figure.width) {
        return true;
    }
    return false;
}

void perform_dragging() {
    if (!drag_state.dragging) {
        return;
    }

    Vector2 mouse = GetMousePosition();
    GridPos mouse_pos = vector2_to_grid_pos(mouse);
    // TODO: snap figure to edge when mouse is out of game grid
    if (validate_mouse_pos(mouse_pos)) {
        drag_state.position_offset = mouse_pos;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        handle_drop(drag_state.position_offset);
        return;
    }

    draw_ghost(grid_pos_to_vector2(drag_state.position_offset));
}
 
