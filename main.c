#include "raylib.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "block-shapes/block_shapes.h"
#include "core.h"

#define SPREAD_CHANCE 50

void print_position(GridPos pos) {
    printf("row: %d col: %d\n", pos.row, pos.col);
}

Grid board = {0};
DragState drag_state = {0};
Figure options_panel[OPTIONS_COUNT];

void copy_board(Grid dst, Grid src) {
    memcpy(dst, src, GRID_ROWS * GRID_COLS * sizeof(Pixel));
}

GridPos vector2_to_grid_pos(Vector2 vector) {
    return (GridPos){.row = vector.y / PIXEL_SIZE, .col = vector.x / PIXEL_SIZE};
}

Vector2 grid_pos_to_vector2(GridPos pos) {
    return (Vector2){.x = pos.col * PIXEL_SIZE, .y = pos.row * PIXEL_SIZE};
}

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

int is_on_which_square(GridPos mouse_pos) {
    for (int i = 0; i < OPTIONS_COUNT; i++) {
        Figure fig = options_panel[i];
        bool is_overlapping = (mouse_pos.row >= fig.pos.row) && (mouse_pos.row < fig.pos.row + fig.total_row) &&
                              (mouse_pos.col >= fig.pos.col) && (mouse_pos.col < fig.pos.col + fig.total_col);
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

void draw_ghost(Vector2 mouse) {
    for (int i = 0; i < SQUARE_BLOCK_SIZE; i++) {
        for (int j = 0; j < SQUARE_BLOCK_SIZE; j++) {
            int x = mouse.x + (j * PIXEL_SIZE);
            int y = mouse.y + (i * PIXEL_SIZE);
            DrawRectangle(x, y, 5, 5, drag_state.ghost_figure.color);
        }
    }
}

bool collides_with_game_grid(GridPos mouse_pos) {
    for (int i = 0; i < SQUARE_BLOCK_SIZE; i++) {
        for (int j = 0; j < SQUARE_BLOCK_SIZE; j++) {
            int row = mouse_pos.row + i;
            int col = mouse_pos.col + j;
            if (board[row][col].active) {
                return true;
            }
        }
    }
    return false;
}

void handle_drop(GridPos mouse_pos) {
    if (mouse_pos.row < GRID_ROWS && !collides_with_game_grid(mouse_pos)) {
        create_square_block(mouse_pos, drag_state.ghost_figure.color);
    } else {
        options_panel[drag_state.fig_idx] = drag_state.ghost_figure;
        drag_state = (DragState){0};
    }
    drag_state = (DragState){0};
}

bool validate_mouse_pos(GridPos mouse_pos) {
    int row = mouse_pos.row;
    int col = mouse_pos.col;
    if (row >= 0 && row < SCREEN_ROWS && col >= 0 && col <= SCREEN_COLS - SQUARE_BLOCK_SIZE) {
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
    if (validate_mouse_pos(mouse_pos)) {
        drag_state.position_offset = mouse_pos;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        handle_drop(drag_state.position_offset);
        return;
    }

    draw_ghost(grid_pos_to_vector2(drag_state.position_offset));
}

// TODO: select figures and colors by random choice
void generate_options() {
    int options_panel_col_width = OPTIONS_COLS / OPTIONS_COUNT;

    for (int i = 0; i < OPTIONS_COUNT; i++) {
        options_panel[i] = (Figure){
            .color = ORANGE,
            .type = BLOCK_SQUARE,
            .pos = (GridPos){.row = GRID_ROWS + 1, .col = i * options_panel_col_width},
            .total_row = SQUARE_BLOCK_SIZE,
            .total_col = SQUARE_BLOCK_SIZE,
            .active = true,
        };
    }
}

void render_options_panel() {
    for (int i = 0; i < OPTIONS_COUNT; i++) {
        // TODO: add more shapes
        switch (options_panel[i].type) {
        case BLOCK_SQUARE:
            create_square_figure(options_panel[i].pos, options_panel[i].color);
            break;
        default:
            break;
        }
    }
}

int main(void) {
    printf("gooning simulator\n");
    const int pixel_width = PIXEL_SIZE;
    const int pixel_height = PIXEL_SIZE;
    const int window_width = SCREEN_COLS * pixel_width;
    const int window_height = SCREEN_ROWS * pixel_height;

    InitWindow(window_width, window_height, "gooning simulator");
    SetTargetFPS(60);

    GridPos square_pos = {.col = GRID_COLS / 2, .row = 3 * GRID_ROWS / 4};

    create_square_block(square_pos, GREEN);
    // create_L_block(square_pos, GREEN);
    generate_options();

    float timer = 0.0f;
    float delay = 0.005f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        timer += dt;

        if (timer >= delay && square_pos.row < GRID_ROWS) {
            timer = 0;
            GridPos new_square_pos = {.row = square_pos.row + 1, .col = square_pos.col};
            square_pos = new_square_pos;
        }
        update_pixels();

        BeginDrawing();
        ClearBackground(DARKGRAY);
        // render game grid
        for (int row = 0; row < GRID_ROWS; row++) {
            for (int col = 0; col < GRID_COLS; col++) {
                if (board[row][col].active) {
                    DrawRectangle(col * pixel_width, row * pixel_height, 5, 5, board[row][col].color);
                }
            }
        }

        DrawLine(0, GRID_ROWS * PIXEL_SIZE, window_width, GRID_ROWS * PIXEL_SIZE, GREEN);

        // render figure options below game grid
        render_options_panel();

        detect_dragging();
        perform_dragging();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
