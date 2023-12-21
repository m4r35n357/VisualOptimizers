
CFLAGS=-std=c99 -O3 -fno-math-errno -flto
WARNINGS=-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Wredundant-decls -Wmissing-declarations

ifeq ($(CCC),clang)
  CC=/usr/bin/clang
else  # default
  CC=/usr/bin/gcc
  CFLAGS += -s
  WARNINGS += -Wunsuffixed-float-constants
endif

%.o: %.c
	$(CC) $(CFLAGS) -MT $@ -MMD -MP -c -o $@ $< $(WARNINGS)

all: nm-ackley nm-hartmann3 nm-hartmann6 nm-rosenbrock nm-himmelblau nm-beale nm-sphere

nm-%: %.o nelder_mead.o main.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

.PHONY: test ctags clean depclean

test: all
	@./nm-ackley 9 0 0 1.0 -2.10 -3.04 4.50 
	@./nm-sphere 9 0 0 1.0 -2.10 -3.04 4.50
	@./nm-hartmann3 9 0 0 1.0 0.5 0.5 0.5
	@#./nm-hartmann6 9 0 0 1.0 0.5 0.5 0.5 0.5 0.5 0.5
	@./nm-rosenbrock 9 0 0 1.0 1.0 0.0
	@./nm-rosenbrock 9 0 0 1.0 -1.0 0.0
	@./nm-rosenbrock 9 0 0 1.0 -1.0 1.0
	@./nm-himmelblau 9 0 0 1.0  3.0  3.0
	@./nm-himmelblau 9 0 0 1.0  3.0 -3.0
	@./nm-himmelblau 9 0 0 1.0 -3.0  3.0
	@./nm-himmelblau 9 0 0 1.0 -3.0 -3.0
	@./nm-beale 9 0 0 1.0 0.0 0.0

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf nm-* *.o

depclean: clean
	@rm -f *.d

-include *.d
