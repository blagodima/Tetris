#include "front.h"

int main() { tetris_run(); }

WINDOW *create_newwin(int height, int width, int start_y, int start_x) {
  WINDOW *local_win;

  local_win = newwin(height, width, start_y, start_x);
  box(local_win, 0, 0);
  wrefresh(local_win);

  return local_win;
}

void destroy_win(WINDOW *local_win) {
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(local_win);
  delwin(local_win);
}

void tetris_run() {
  GameInfo_t game;  //= updateCurrentState();
  srand(time(NULL));  // Инициализация генератора случайных чисел
  initscr();
  raw();
  nonl();
  noecho();

  WINDOW *my_win;
  WINDOW *my_win2;
  int start_x, start_y, width, height;

  cbreak();
  curs_set(0);
  keypad(stdscr, TRUE);

  height = width = 22;
  start_y = start_x = 0;

  refresh();
  my_win2 = create_newwin(height, width, start_y, start_x + 21);
  my_win = create_newwin(height, width, start_y, start_x);

  int start_status = 1;
  int game_status = 0;

  while (start_status != 0) {
    game = update_current_state();
    clear_end_res_game(my_win);
    if (game.speed == -1) {
      game_status = 0;
    }

    if (game_status) {
      process_ascii_gui(my_win, &game);
      process_data_gui(my_win2, &game);
    } else {
      start_menu(my_win, my_win2);
    }
    input_from_keyboard(&start_status, &game_status);
  }

  destroy_win(my_win);
  destroy_win(my_win2);
  endwin(); /* End curses mode		  */
  free(game.field);
}

void clear_end_res_game(WINDOW *win) {
  for (int i = 1; i < BOARD_HEIGHT + 1; i++) {
    mvwaddstr(win, i, 1, "                    ");
  }
}

void start_menu(WINDOW *win, WINDOW *win_2) {
  clear_end_res_game(win);
  char *str1 = "Press \"S\"";
  mvwaddstr(win, 9, 7, str1);
  char *str2 = "to start the game";
  mvwaddstr(win, 11, 3, str2);
  char *str3 = "Brick Game";
  mvwaddstr(win_2, 9, 6, str3);
  char *str4 = "Tetris";
  mvwaddstr(win_2, 11, 8, str4);

  wrefresh(win);
  wrefresh(win_2);
}

void input_from_keyboard(int *start_status, int *game_status) {
  nodelay(stdscr, TRUE);
  *start_status = 1;

  int c = getch();

  switch (c) {
    case KEY_LEFT:
      user_input(Left, false);
      break;
    case KEY_RIGHT:
      user_input(Right, false);
      break;
    case KEY_DOWN:
      user_input(Down, false);
      break;
    case KEY_UP:
      user_input(Up, false);
      break;
    case 's':
      user_input(Start, false);
      *game_status = 1;
      break;
    case 'p':
      user_input(Pause, false);
      break;
    case ' ':
      user_input(Action, false);
      break;
    case 'q':
      user_input(Terminate, false);
      *start_status = 0;
      break;
  }

  return;
}

void process_ascii_gui(WINDOW *win, GameInfo_t *game) {
  int **frame = mem_alloc(COLS_COUNT, COLS_COUNT);
  convert_marix_to_frame(game->field, frame, COLS_COUNT, ROWS_MATRIX);

  for (int i = 0; i < COLS_COUNT; i++) {
    for (int j = 0; j < COLS_COUNT; j++) {
      mvwaddch(win, i + 1, j + 1, frame[i][j]);
    }
  }

  if (game->pause) {
    mvwaddstr(win, 9, 2, "                ");
    mvwaddstr(win, 10, 2, "    P A U S E   ");
    mvwaddstr(win, 11, 2, "                ");
  }

  free(frame);
  wrefresh(win);
}

void process_data_gui(WINDOW *win, GameInfo_t *game) {
  const int MAX_RSCR_SIZE = 4;
  int **frame = mem_alloc(8, 8);
  for (int i = 1; i < BOARD_HEIGHT; i++) {
    mvwaddstr(win, i, 1, "                   ");
  }
  convert_marix_to_frame(game->next, frame, MAX_RSCR_SIZE, MAX_RSCR_SIZE);
  for (int i = 3; i < MAX_RSCR_SIZE + 3; i++) {
    for (int j = 3; j < (MAX_RSCR_SIZE * 2) + 3; j++) {
      mvwaddch(win, i + 1, j + 4, frame[i - 3][j - 3]);
    }
  }
  char *next_str = " ";
  for (int i = 6; i < (int)strlen(next_str) + 6; i++) {
    mvwaddch(win, 3, i + 1, next_str[i - 6]);
  }
  char score_str[20];
  sprintf(score_str, "Score: %d", game->score);
  mvwaddstr(win, 14, 6, score_str);
  sprintf(score_str, "Record: %d", game->record);
  mvwaddstr(win, 16, 5, score_str);
  sprintf(score_str, "Level: %d", game->level);
  mvwaddstr(win, 18, 6, score_str);

  free(frame);
  wrefresh(win);
}

void convert_marix_to_frame(int **matrix, int **frame, int size_l, int size_w) {
  for (int j = 0; j < size_l; j++) {
    int frame_i = 0;
    for (int i = 0; i < size_w; i++) {
      if (matrix[i][j]) {
        frame[j][frame_i] = (int)'[';
        frame[j][frame_i + 1] = (int)']';
      } else {
        frame[j][frame_i] = (int)' ';
        frame[j][frame_i + 1] = (int)' ';
      }
      frame_i += 2;
    }
  }
}

char **make_interface() {
  char **result = (char **)malloc(sizeof(char) * 10 * 20 + sizeof(char *) * 10);

  return result;
}
