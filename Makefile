#
#  (c) 2018-2023 m4r35n357@gmail.com (Ian Smith), for licencing see the LICENCE file
#

CFLAGS=-std=c99 -O3 -fno-math-errno -flto -s
WARNINGS=-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Wredundant-decls -Wmissing-declarations
LIB_STD=-lm
CC=/usr/bin/gcc

%.o: %.c
	$(CC) $(CFLAGS) -MT $@ -MMD -MP -c -o $@ $< $(WARNINGS)

all: nm-ackley-bin nm-sphere-bin

nm-%-bin: %.o nelder_mead.o point.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

.PHONY: test clean ctags

test: all
	time -p ./$(BIN) -2.10 -3.04 4.50 

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf nm-*-bin *.so *.o *.gcda *.gcno coverage* gmon.out

depclean: clean
	@rm -f *.d

-include *.d
