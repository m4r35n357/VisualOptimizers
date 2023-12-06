
FLAGS = -std=c99 -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Wredundant-decls -Wmissing-declarations
CC_OPT = -O3 -ffast-math -fno-common
BIN = nm
SRC = main.c nelder_mead.c point.c ackley.c

all:
	gcc $(FLAGS) $(CC_OPT) -o $(BIN) $(SRC) -lm

.PHONY: test clean ctags

test: all
	time -p ./$(BIN) -2.10 -3.04 4.50 

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf $(BIN) *.so *.o *.gcda *.gcno coverage* gmon.out
