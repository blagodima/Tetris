#include "tetris.h"

static bricks *game;

void user_input(UserAction_t action, bool hold) {
  switch (action) {
    case Start:
      if (game == NULL || !game->is_started) {  // Проверяем, что не NULL
        game_init();
        iteration();
        if (game->tmp != NULL)
          game->tmp->speed = 1;  // Убедимся, что game->tmp не равен NULL
      }
      if (game != NULL) game->is_started = true;
      break;
    case Pause:
      if (game != NULL && game->is_started) game->pause = !game->pause;
      break;
    case Terminate:
      break;
    case Right:
      if (game != NULL && game->is_started && !game->pause && !hold)
        move_figure(MOVE_RIGHT);
      break;
    case Up:
      break;
    case Left:
      if (game != NULL && game->is_started && !game->pause && !hold)
        move_figure(MOVE_LEFT);
      break;
    case Down:
      if (game != NULL && game->is_started && !game->pause)
        move_figure(MOVE_DOWN);
      break;
    case Action:
      if (game != NULL && game->is_started && !game->pause) rotate_figure();
      break;
    default:
      break;
  }
}

GameInfo_t update_current_state() {
  GameInfo_t result = {0};

  if (game != NULL) {
    if (game->is_started) {
      iteration();

      if (game->tmp->speed == -1)
        game_over();  // если -1, запускаем game_over()
    }

    // Заполняем структуру result
    result.field = game == NULL ? NULL : game->matrix_to_show;
    result.next = game == NULL ? NULL : game->tmp->figure_rsrc_next;
    result.level = game == NULL ? 0 : game->level;
    result.record = game == NULL ? 0 : game->record_result;
    result.pause = game == NULL ? false : game->pause;
    result.score = game == NULL ? 0 : game->result;
    result.speed = game == NULL ? -1 : game->tmp->speed;
  } else {
    // Если game равен NULL
    result.field = NULL;
    result.next = NULL;
    result.level = 0;
    result.record = 0;
    result.score = 0;
    result.speed = -1;
    result.pause = false;
  }

  return result;
}

int game_init() {
  game = malloc(sizeof(bricks));
  game->level = 1;
  game->result = 0;
  game->record_result = read_record("record.log");
  game->pause = 0;
  game->is_started = 0;
  game->matrix = mem_alloc(BOARD_WIDTH, BOARD_HEIGHT);
  game->matrix_act = mem_alloc(BOARD_WIDTH, BOARD_HEIGHT);
  game->matrix_to_show = mem_alloc(BOARD_WIDTH, BOARD_HEIGHT);
  for (int j = 0; j < BOARD_HEIGHT; j++) {
    for (int i = 0; i < BOARD_WIDTH; i++) {
      game->matrix[i][j] = 0;
      game->matrix_act[i][j] = 0;
      game->matrix_to_show[i][j] = 0;
    }
  }

  game->tmp = malloc(sizeof(brick_state) * 1);
  game->tmp->is_1_NULL = 1;
  game->tmp->is_2_NULL = 1;
  game->tmp->is_fall = false;
  game->tmp->pos_x = 4;
  game->tmp->pos_y = -1;
  game->tmp->speed = 1;

  return 0;
}

int game_over() {
  if (game == NULL) return -1;  // Защита от многократного вызова game_over

  // Сначала обновим record, если это необходимо.
  if (game->record_result < game->result) {
    write_record("record.log", game->result);
  }

  // Перемещаем присвоение флага is_started перед освобождением памяти.
  game->is_started = false;

  // Освобождаем память game->tmp и проверяем указатели перед освобождением
  if (game->tmp != NULL) {
    free(game->tmp->figure_rsrc_next);
    free(game->tmp->figure_rsrc);
    game->tmp->figure_rsrc = NULL;
    game->tmp->figure_rsrc_next = NULL;
    free(game->tmp);
    game->tmp = NULL;
  }

  // Освобождаем память, связанную с матрицами
  free(game->matrix);
  game->matrix = NULL;

  free(game->matrix_act);
  game->matrix_act = NULL;

  free(game->matrix_to_show);
  game->matrix_to_show = NULL;

  // Наконец, освобождаем сам объект game
  free(game);
  game = NULL;

  return 0;
}

int read_record(const char *file_path) {
  FILE *to_read;

  to_read = fopen(file_path, "r");
  if (to_read == NULL) {
    to_read = fopen(file_path, "a");
    fprintf(to_read, "0");
    fclose(to_read);
    to_read = fopen(file_path, "r");
  }
  char record[255];
  int max = 0;
  int tmp = 0;
  while ((fgets(record, 255, to_read)) != NULL) {
    sscanf(record, "%d", &tmp);
    if (tmp > max) {
      max = tmp;
    }
  }
  fclose(to_read);
  return max;
}

void write_record(const char *file_path, int result) {
  FILE *to_read = fopen(file_path, "a");
  if (to_read != NULL) {
    fprintf(to_read, "\n%d", result);
    fclose(to_read);
  }
}

void rotate_figure_next() {
  int **new_figure = mem_alloc(game->tmp->size_w, game->tmp->size_l);
  for (int i = 0; i < game->tmp->size_w; i++) {
    for (int j = 0; j < game->tmp->size_l; j++) {
      new_figure[game->tmp->size_w - i - 1][j] =
          game->tmp->figure_rsrc_next[j][i];
    }
  }
  free(game->tmp->figure_rsrc_next);
  game->tmp->figure_rsrc_next = new_figure;
}

void rotate_figure() {
  int **new_figure = mem_alloc(game->tmp->size_w, game->tmp->size_l);
  for (int i = 0; i < game->tmp->size_w; i++) {
    for (int j = 0; j < game->tmp->size_l; j++) {
      new_figure[game->tmp->size_w - i - 1][j] = game->tmp->figure_rsrc[j][i];
    }
  }
  check_rotate(new_figure);
}

bool check_rotate(int **vedro) {
  int sum_org = 0;
  int sum_new = 0;
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      sum_org += game->matrix_to_show[j][i];
    }
  }

  int **temp = game->tmp->figure_rsrc;
  game->tmp->figure_rsrc = vedro;
  generate_matrix();

  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      sum_new += game->matrix_to_show[j][i];
    }
  }

  if (sum_org != sum_new) {
    game->tmp->figure_rsrc = temp;
    generate_matrix();
    free(vedro);
  } else {
    free(temp);
  }

  return 1;
}

void generate_figure() {
  int rand_n = 0;
  if (game != NULL && game->tmp != NULL && !game->tmp->is_fall) {
    game->tmp->is_fall = true;
    if (!game->tmp->is_2_NULL) {
      int tmp = 0;
      for (int i = 0; i < 4; i++) {
        for (int j = i; j < 4; j++) {
          tmp = game->tmp->figure_rsrc_next[i][j];
          game->tmp->figure_rsrc_next[i][j] = game->tmp->figure_rsrc_next[j][i];
          game->tmp->figure_rsrc_next[j][i] = tmp;
        }
      }
      copy_rsrc_next();
      game->tmp->is_1_NULL = 0;
    }

    // Генерация случайного числа
    rand_n = rand() % 7;

    switch (rand_n) {
      case 0:
        create_figure(game->tmp, FIGURE_T);
        break;
      case 1:
        create_figure(game->tmp, FIGURE_O);
        break;
      case 2:
        create_figure(game->tmp, FIGURE_S);
        break;
      case 3:
        create_figure(game->tmp, FIGURE_Z);
        break;
      case 4:
        create_figure(game->tmp, FIGURE_I);
        break;
      case 5:
        create_figure(game->tmp, FIGURE_J);
        break;
      case 6:
        create_figure(game->tmp, FIGURE_L);
        break;
    }
    game->tmp->is_2_NULL = 0;
    rand_n = rand() % 4;
    const int MAX_ROTATE = 4;
    for (int i = rand_n; i < MAX_ROTATE; i++) rotate_figure_next();
  }
}

void copy_rsrc_next() {
  if (game != NULL && game->tmp != NULL && game->tmp->is_1_NULL != 1)
    free(game->tmp->figure_rsrc);
  game->tmp->figure_rsrc = game->tmp->figure_rsrc_next;
  game->tmp->figure_rsrc_next = NULL;
  game->tmp->is_2_NULL = 1;
}

void move_figure(type_move type) {
  if (type == MOVE_LEFT && !check_sides(MOVE_LEFT)) {
    game->tmp->pos_x--;
  } else if (type == MOVE_RIGHT && !check_sides(MOVE_RIGHT)) {
    game->tmp->pos_x++;
  } else if (type == MOVE_DOWN && !check_fall()) {
    game->tmp->pos_y++;
  } else if (type == MOVE_DOWN) {
    put_figure_stat(game);
  }
  if (game->tmp->speed != -1) generate_matrix();
}

void fall_figure() {
  static int count = 0;
  const int MAX_LEVEL = 11;
  if ((count++ % ((MAX_LEVEL - game->tmp->speed) * 1500)) == 0)
    move_figure(MOVE_DOWN);
  if (count >= (((MAX_LEVEL - game->tmp->speed) * 1500) + 1)) count = 1;
}

void iteration() {
  if (game == NULL || game->pause) return;
  if (game->tmp->is_1_NULL == 1) {
    generate_figure();
    game->tmp->is_fall = false;
  }

  generate_figure();
  generate_matrix();
  fall_figure();
}

void generate_matrix() {
  int start_y = game->tmp->pos_y;
  int start_x = game->tmp->pos_x;
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      if (start_y <= i && i < (start_y + game->tmp->size_l) && start_x <= j &&
          j < (start_x + game->tmp->size_w)) {
        if (game->tmp->figure_rsrc[i - start_y][j - start_x] == 1) {
          game->matrix_act[j][i] = 1;
        } else {
          game->matrix_act[j][i] = 0;
        }
      } else {
        game->matrix_act[j][i] = 0;
      }
    }
  }

  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      game->matrix_to_show[j][i] =
          game->matrix[j][i] == 1 || game->matrix_act[j][i] == 1;
    }
  }
}

int check_sides(type_move side) {
  int result = false;

  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      if ((j == 0 && game->matrix_act[j][i] == 1 && (side == MOVE_LEFT)) ||
          (game->matrix_act[j][i] == 1 && (side == MOVE_LEFT) &&
           (j < 9 && game->matrix_act[j - 1][i] != 1 &&
            game->matrix[j - 1][i] == 1))) {
        result = true;
      }
      if ((j == 9 && game->matrix_act[j][i] == 1 && (side == MOVE_RIGHT)) ||
          (game->matrix_act[j][i] == 1 && (side == MOVE_RIGHT) &&
           (j > 0 && game->matrix_act[j + 1][i] != 1 &&
            game->matrix[j + 1][i] == 1))) {
        result = true;
      }
    }
  }
  return result;
}

int check_fall() {
  int result = false;
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      if (game->matrix_act[j][i] == 1 &&
          ((i < 19 && game->matrix_act[j][i + 1] != 1 && i < 19 &&
            game->matrix[j][i + 1] == 1) ||
           (i == 19))) {
        result = true;
      }
    }
  }
  return result;
}

void put_figure_stat() {
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      game->matrix[j][i] =
          game->matrix[j][i] == 1 || game->matrix_act[j][i] == 1;
    }
  }

  game->result += check_rows(game->matrix);
  game->level = game->result / 600 + 1 > 10 ? 10 : game->result / 600 + 1;
  game->tmp->speed = game->level;
  game->tmp->speed = check_end(game->matrix) ? -1 : game->tmp->speed;

  game->tmp->pos_x = 4;
  game->tmp->pos_y = -1;
  game->tmp->is_fall = false;

  if (game->tmp->speed != -1) generate_figure();
}

bool check_end(int **matrix) {
  bool result = false;

  for (int i = 0; i < BOARD_WIDTH; i++) {
    if (matrix[i][0] == 1) result = true;
  }

  return result;
}

int check_rows(int **matrix_stat) {
  int count_strs_del = 0;
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    int sum = 0;
    for (int j = 0; j < BOARD_WIDTH; j++) {
      sum += matrix_stat[j][i];
    }
    if (sum >= 10) {
      remove_row(matrix_stat, i);
      count_strs_del++;
    }
  }
  int result = 0;
  switch (count_strs_del) {
    case 1:
      result = 100;
      break;
    case 2:
      result = 300;
      break;
    case 3:
      result = 700;
      break;
    case 4:
      result = 1500;
      break;
  }
  return result;
}

void remove_row(int **matrix_stat, int idx) {
  for (size_t i = (size_t)idx; i > 0; i--) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      matrix_stat[j][i] = matrix_stat[j][i - 1];
    }
  }
}

int **mem_alloc(int rows, int cols) {
  int **data = (int **)malloc(rows * cols * sizeof(int) + rows * sizeof(int *));
  int *ptr = (int *)(data + rows);
  for (int i = 0; i < rows; i++) {
    data[i] = ptr + cols * i;
  }
  return data;
}

void create_figure(brick_state *settings, FigureType type) {
  if (settings->is_2_NULL != 1) {
    free(settings->figure_rsrc_next);
  }

  int figure_templates[7][4][4] = {
      // FIGURE_O
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
      // FIGURE_L
      {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
      // FIGURE_J
      {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
      // FIGURE_T
      {{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
      // FIGURE_S
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
      // FIGURE_Z
      {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
      // FIGURE_I
      {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}};

  settings->figure_rsrc_next = mem_alloc(4, 4);
  settings->is_2_NULL = 0;
  settings->size_w = 4;
  settings->size_l = 4;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      settings->figure_rsrc_next[i][j] = figure_templates[type][i][j];
    }
  }
}