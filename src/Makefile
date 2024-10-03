CC = gcc
CFLAGS = -Wall -Werror -Wextra
SRC_DIR = ./brick_game/tetris/tetris.c ./gui/cli/front.c
GCOV_FLAGS=-fprofile-arcs -ftest-coverage

ifeq ($(shell uname), Darwin)
	LIBS = $(shell pkg-config --libs check) -lm
	OPENF = open
else
	LIBS = -lcheck -lsubunit -lrt -lm -lpthread
	OPENF = xdg-open
endif

.PHONY: test dvi

all: gcov_report install

reinstall:
	@rm -rf build && mkdir build
	@$(CC) $(CFLAGS) $(SRC_DIR) -g -o ./build/Tetris -lncurses 
	@./build/Tetris && echo " \n *** Brick Game Tetris has been launched ***" && echo " "

s21_tetris.a:
	@echo " \n*** Building the library s21_tetris.a ***"
	$(CC) $(CFLAGS) -c $(SRC_DIR)
	ar rcs s21_tetris.a *.o
	rm -rf *.o

test: s21_tetris.a
	@echo " \n*** Running tests ***"
	$(CC) -g $(CFLAGS) tests/tests.c -L. s21_tetris.a -o testing $(LIBS)
	./testing && echo " "

gcov_report: CFLAGS += $(GCOV_FLAGS)
gcov_report: test
	@echo "*** Running gcov_report ***"
	lcov -t "testing" -o testing.info -c -d .
	genhtml -o report testing.info
	@$(OPENF) report/brick_game/tetris/index.html

valgrind: clean s21_tetris.a
	@$(CC) -g $(CFLAGS) tests/tests.c -L. s21_tetris.a -o testing $(LIBS)
	@echo " \n*** Valgrind started checking the code and will show you the results after that ***"
	@valgrind --tool=memcheck --leak-check=yes --log-file="valgrind.log" ./testing && echo " "
	@$(OPENF) valgrind.log

leaks: clean s21_tetris.a
	@$(CC) -g $(CFLAGS) tests/tests.c -L. s21_tetris.a -o testing $(LIBS)
	@leaks -atExit -- ./testing
	@echo " \n*** Command 'leaks' started checking the code ***"

style_check:
	@echo " \n*** This code has been checked for Google Style ***" && echo " "
	@clang-format --style=Google -n */*/*.c */*/*.h

style_set:
	@echo " \n*** This code has been formatted with Google Style ***" && echo " "
	@clang-format --style=Google -i */*/*.c */*/*.h

install:
	@mkdir build
	@$(CC) $(CFLAGS) $(SRC_DIR) -g -o ./build/Tetris -lncurses 
	@echo " \n *** Brick Game Tetris has been installed to build/Teris ***" && echo " "

uninstall:
	@rm -rf build
	@echo " \n*** Brick Game Tetris has been uninstalled from build/Teris ***" && echo " "

clean:
	@rm -rf html build report testing.dSYM build/*.dSYM
	@rm -f *.gcda *.gcno *.tar *.info *.out *.a *.o *.tar *.log testing  \
	documentation* dvi/*.dvi dvi/*.out dvi/*.log dvi/*.aux dvi/*.pdf
	@echo " \n*** The trash has been cleaned ***" && echo " "

dist: 
	@mkdir build
	@$(CC) $(CFLAGS) $(SRC_DIR) -g -o ./build/Tetris -lncurses 
	@tar -cvf brick_game_tetris.tar ./build
	@echo " \n*** Brick Game Tetris has been zipped in brick_game_tetris.tar ***" && echo " "

doc_deps:
	sudo apt install texlive-latex-extra texlive-extra-utils
	sudo apt install texline-lang-cyrillic

# dvi:
# 	doxygen Doxyfile
# 	open html/index.html

clean_mem:
	sudo du -sh /var/cache/apt
	sudo apt-get autoclean
	sudo apt-get clean
	sudo du -sh /var/cache/apt

TEX=./dvi/documentation
TARGET=./dvi/documentation
LATEX_ARGS=--interaction batchmode
LATEX=latex

$(TARGET).pdf: $(TEX).tex
	pdflatex $(LATEX_ARGS) $(TEX)
	mv documentation* ./dvi

$(TARGET).dvi: $(TEX).tex
	$(LATEX) $(LATEX_ARGS) $(TEX)
	mv documentation* ./dvi

debug:
	latex $(TEX)

pdfshow: $(TARGET).pdf
#	xpdf $(TARGET).pdf

acroshow: $(TARGET).pdf
#	acroread $(TARGET).pdf

dvi: $(TARGET).dvi
	xdvi $(TARGET).dvi
