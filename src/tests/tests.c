#include "tests.h"

START_TEST(test_score) {
  GameInfo_t gi = update_current_state();
  user_input(Start, false);
  gi = update_current_state();
  const int HOW_DEEP = 4;
  for (int i = 0; i < HOW_DEEP; i++) {
    user_input(Down, false);
    gi = update_current_state();
  }
  ck_assert_int_eq(gi.score, 0);

  user_input(Terminate, false);
}
END_TEST

START_TEST(test_level) {
  GameInfo_t gi = update_current_state();
  user_input(Start, false);
  gi = update_current_state();
  ck_assert_int_eq(gi.level, 1);

  user_input(Terminate, false);
}
END_TEST

START_TEST(test_pause) {
  GameInfo_t gi = update_current_state();
  user_input(Start, false);
  gi = update_current_state();

  ck_assert_int_eq(gi.pause, false);

  user_input(Terminate, false);
}
END_TEST

START_TEST(test_moving_right) {
  GameInfo_t gi = update_current_state();
  user_input(Start, false);
  gi = update_current_state();
  const int HOW_DEEP = 4;
  for (int i = 0; i < HOW_DEEP; i++) {
    user_input(Down, false);
    gi = update_current_state();
  }
  int plan_val = 0;
  int end_val = 0;
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      plan_val += gi.field[j][i] * (j + 1);
    }
  }
  user_input(Right, false);
  gi = update_current_state();

  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      end_val += gi.field[j][i] * j;
    }
  }
  ck_assert_int_eq(plan_val, end_val);

  user_input(Terminate, false);
}
END_TEST

START_TEST(test_moving_left) {
  GameInfo_t gi = update_current_state();
  user_input(Start, false);
  gi = update_current_state();
  const int HOW_DEEP = 4;
  for (int i = 0; i < HOW_DEEP; i++) {
    user_input(Down, false);
    gi = update_current_state();
  }
  int plan_val = 0;
  int end_val = 0;
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      plan_val += gi.field[j][i] * (j - 1);
    }
  }
  user_input(Left, false);
  gi = update_current_state();

  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      end_val += gi.field[j][i] * j;
    }
  }
  ck_assert_int_eq(plan_val, end_val);

  user_input(Terminate, false);
}
END_TEST

START_TEST(test_put_figure) {
  GameInfo_t gi = update_current_state();
  user_input(Start, false);
  gi = update_current_state();

  bool is_reach_bottom = false;
  do {
    user_input(Down, false);
    gi = update_current_state();
    for (int i = 0; i < BOARD_WIDTH && !is_reach_bottom; i++) {
      is_reach_bottom = gi.field[i][19] == 1;
    }
  } while (!is_reach_bottom);
  int before = 0;
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      before += gi.field[j][i];
    }
  }

  const int HOW_DEEP = 4;
  for (int i = 0; i < HOW_DEEP; i++) {
    user_input(Down, false);
    gi = update_current_state();
    user_input(Action, false);
    gi = update_current_state();
  }

  int after = 0;
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      after += gi.field[j][i];
    }
  }

  ck_assert_int_gt(after, before);

  user_input(Terminate, false);
}
END_TEST

START_TEST(test_gameover) {
  GameInfo_t gi = update_current_state();
  user_input(Start, false);
  gi = update_current_state();

  int count = 0;
  do {
    if (gi.speed == -1) {
      user_input(Start, false);
      gi = update_current_state();
      count++;
      usleep(30000);
    }
    user_input(Down, false);
    gi = update_current_state();

  } while (count < 400 || gi.speed != -1);

  ck_assert_int_eq(gi.speed, -1);

  user_input(Terminate, false);
}
END_TEST

START_TEST(test_write_record) {
    const char *file_path = "temp_test_file.txt";
    int test_result = 42;

    // Очищаем или создаём временный файл перед тестом
    FILE *test_file = fopen(file_path, "w");
    fail_unless(test_file != NULL);
    fclose(test_file);

    // Вызываем функцию
    write_record(file_path, test_result);

    // Проверяем, что результат был правильно записан в файл
    test_file = fopen(file_path, "r");
    fail_unless(test_file != NULL);

    int read_result;
    int scanned_count = fscanf(test_file, "%d", &read_result);
    fclose(test_file);

    // Проверяем, что мы смогли прочитать одно число и что оно равно ожидаемому
    ck_assert_int_eq(scanned_count, 1);
    ck_assert_int_eq(read_result, test_result);

    // Удаляем тестовый файл
    remove(file_path);
}
END_TEST

Suite *tetris_suite() {
  Suite *s = suite_create("tetris_suite");
  TCase *tc = tcase_create("tetris_tc");

  tcase_add_test(tc, test_score);
  tcase_add_test(tc, test_level);
  tcase_add_test(tc, test_pause);
  tcase_add_test(tc, test_moving_right);
  tcase_add_test(tc, test_moving_left);
  tcase_add_test(tc, test_put_figure);
  tcase_add_test(tc, test_gameover);
  tcase_add_test(tc, test_write_record);

  suite_add_tcase(s, tc);

  return s;
}

int main(void) {
  tests_launcher();

  return 0;
}

void tests_launcher(void) {
  Suite *tests_list[] = {tetris_suite(), NULL};

  for (Suite **current = tests_list; *current != NULL; current++) {
    testcase_launcher(*current);
  }
}

void testcase_launcher(Suite *testcase) {
  static int counter = 1;

  if (counter > 1) putchar('\n');
  printf("%d ", counter);
  counter++;

  SRunner *sr = srunner_create(testcase);

  srunner_set_fork_status(sr, CK_NOFORK);
  srunner_run_all(sr, CK_NORMAL);

  srunner_free(sr);
}
