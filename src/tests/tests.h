#ifndef TESTS_H
#define TESTS_H

#include <check.h>
#include <ncurses.h>
#include <unistd.h>

#include "../brick_game/tetris/tetris.h"

void tests_launcher(void);
Suite *tetris_suite();
void testcase_launcher(Suite *testcase);

#endif  // TESTS_H