#include "core.h"

static int is_on_which_square(GridPos mouse_pos) {
    int slot_width = SCREEN_COLS / OPTIONS_COUNT;

    for (int i = 0; i < OPTIONS_COUNT; i++) {
        Figure fig = options_panel[i];

        // adjust for the centered figures
        int fig_col_start = (slot_width / 2) - (fig.width / 2);

        int row_start = fig.pos.row;
        int row_end = fig.pos.row + fig.height;
        int col_start = fig.pos.col + fig_col_start;
        int col_end = col_start + fig.width;

        bool is_overlapping = (mouse_pos.row >= row_start && mouse_pos.row < row_end) &&
                              (mouse_pos.col >= col_start && mouse_pos.col < col_end);

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
    int fig_idx = is_on_which_square(mouse_pos);
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
    int vert_mid = fig.height / 2;
    int horiz_mid = fig.width / 2;
    for (int i = 0; i < fig.height; i++) {
        for (int j = 0; j < fig.width; j++) {
            if (fig.mask[i][j]) {
                int x = mouse.x + ((j - horiz_mid) * PIXEL_SIZE);
                int y = mouse.y + ((i - vert_mid) * PIXEL_SIZE);
                DrawRectangle(x, y, 5, 5, fig.color);
            }
        }
    }
}

static bool collides_with_game_grid(GridPos mouse_pos) {
    int fig_width = drag_state.ghost_figure.width;
    int fig_height = drag_state.ghost_figure.height;
    int vert_mid = fig_height / 2;
    int horiz_mid = fig_width / 2;
    for (int i = 0; i < fig_height; i++) {
        for (int j = 0; j < fig_width; j++) {
            int row = mouse_pos.row + (i - vert_mid);
            int col = mouse_pos.col + (j - horiz_mid);
            if (row < GRID_ROWS && col < GRID_COLS && board[row][col].active) {
                return true;
            }
        }
    }
    return false;
}

static void handle_drop(GridPos mouse_pos) {
    int vert_mid = drag_state.ghost_figure.height / 2;
    if (mouse_pos.row + vert_mid < GRID_ROWS && !collides_with_game_grid(mouse_pos)) {
        Figure fig = drag_state.ghost_figure;
        int vert_mid = fig.height / 2;
        int horiz_mid = fig.width / 2;
        for (int i = 0; i < MAX_FIGURE_ROW; i++) {
            for (int j = 0; j < MAX_FIGURE_COL; j++) {
                if (fig.mask[i][j]) {
                    int row = mouse_pos.row + (i - vert_mid);
                    int col = mouse_pos.col + (j - horiz_mid);
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

static inline void snap_to_bounds(GridPos mouse_pos) {
    int row = mouse_pos.row;
    int col = mouse_pos.col;
    int vert_mid = drag_state.ghost_figure.height / 2;
    int horiz_mid = drag_state.ghost_figure.width / 2;

    drag_state.position_offset.row = row;
    drag_state.position_offset.col = col;

    if (col + horiz_mid > SCREEN_COLS) {
        drag_state.position_offset.col = SCREEN_COLS - horiz_mid;
    } else if (col - horiz_mid < 0) {
        drag_state.position_offset.col = horiz_mid;
    }

    if (row + vert_mid > SCREEN_ROWS) {
        drag_state.position_offset.row = SCREEN_ROWS - vert_mid;
    } else if (row - vert_mid < 0) {
        drag_state.position_offset.row = vert_mid;
    }
}

static bool validate_mouse_pos(GridPos mouse_pos) {
    int row = mouse_pos.row;
    int col = mouse_pos.col;
    int vert_mid = drag_state.ghost_figure.height / 2;
    int horiz_mid = drag_state.ghost_figure.width / 2;
    if (row - vert_mid >= 0 && row + vert_mid < SCREEN_ROWS && col - horiz_mid >= 0 && col + horiz_mid <= SCREEN_COLS) {
        return true;
    }

    snap_to_bounds(mouse_pos);
    return false;
}

void perform_dragging() {
    if (!drag_state.dragging) {
        return;
    }

    Vector2 mouse = GetMousePosition();
    GridPos mouse_pos = vector2_to_grid_pos(mouse);
    if (validate_mouse_pos(mouse_pos)) {
        drag_state.position_offset = mouse_pos;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        handle_drop(drag_state.position_offset);
        return;
    }

    draw_ghost(grid_pos_to_vector2(drag_state.position_offset));
}
