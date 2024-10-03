#ifndef FRONT_H
#define FRONT_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

#include "../../brick_game/tetris/tetris.h"

#define COLS_COUNT 20
#define ROWS_MATRIX 10

void tetris_run();
void input_from_keyboard(int *start_status, int *game_status);
void process_ascii_gui(WINDOW *win, GameInfo_t *game);
void process_data_gui(WINDOW *win, GameInfo_t *game);
void start_menu(WINDOW *win, WINDOW *win_2);
void convert_marix_to_frame(int **matrix, int **frame, int size_l, int size_w);
void clear_end_res_game(WINDOW *win);

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

#endif  // FRONT_H