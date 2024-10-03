#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

#define MOVE_LEFT -1
#define MOVE_RIGHT 1
#define MOVE_DOWN 0

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef enum {
  FIGURE_O,
  FIGURE_L,
  FIGURE_J,
  FIGURE_T,
  FIGURE_S,
  FIGURE_Z,
  FIGURE_I,
} FigureType;

typedef struct {
  int **field;
  int **next;
  int score;
  int record;
  int level;
  int speed;
  int pause;
} GameInfo_t;

GameInfo_t update_current_state();

typedef int type_move;

typedef struct brick_state {
  int is_1_NULL;
  int is_2_NULL;
  int **figure_rsrc;
  int **figure_rsrc_next;
  int size_w;
  int size_l;
  int pos_y;
  int pos_x;
  int speed;
  int is_fall;
} brick_state;

typedef struct bricks {
  int **matrix;
  int **matrix_act;
  int **matrix_to_show;
  int result;
  int record_result;
  int level;
  int pause;
  int is_started;
  const char *file_path;
  brick_state *tmp;
} bricks;

void user_input(UserAction_t action, bool hold);
int **mem_alloc(int rows, int cols);
int check_fall();
void create_figure(brick_state *settings, FigureType type);
void copy_rsrc_next();

int check_rows(int **matrix_to_show);

void write_record(const char *file_path, int result);
int read_record(const char *file_path);

int game_init();
int game_over();
void rotate_figure();
void rotate_figure_next();
bool check_rotate(int **vedro);
void generate_figure();
void move_figure(type_move type);
void fall_figure();
void iteration();
void generate_matrix();
int check_sides(type_move side);
int check_fall();
void put_figure_stat();
int check_rows(int **matrix_stat);
void remove_row(int **matrix_stat, int idx);
bool check_end(int **matrix);

#endif  // TETRIS_H