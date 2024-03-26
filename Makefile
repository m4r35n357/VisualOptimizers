
CFLAGS=-std=c99 -O3 -fno-math-errno -flto
WARNINGS=-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Wredundant-decls -Wmissing-declarations
LIB_STD=-lm
LIB_GL=-lGLEW -lglut -lGLU -lGL

ifeq ($(CCC),gcc)  # fast option
  CC=/usr/bin/gcc
  CFLAGS += -s
  WARNINGS += -Wunsuffixed-float-constants
else ifeq ($(CCC),clang)  # fast option
  CC=/usr/bin/clang
else  # default for IDEs and git commits
  CC=/usr/bin/clang
  CFLAGS=-std=c99 -O0 -g
endif

%.o: %.c
	$(CC) $(CFLAGS) -MT $@ -MMD -MP -c -o $@ $< $(WARNINGS)

all: nogl nm-gl spiral-gl

nogl: nm-std spiral-std ctags

nm-%-std: %.o nelder_mead.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

nm-std: nm-levy-std nm-easom-std nm-michalewicz-std nm-rastrigin-std nm-sqrt-std nm-ackley-std nm-rosenbrock-std nm-dixon-price-std nm-st-std nm-box-std nm-schwefel-std nm-trid-std

nm-%-gl: %.o nelder_mead.o opengl.o simplex-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

nm-gl: nm-levy-gl nm-easom-gl nm-michalewicz-gl nm-rastrigin-gl nm-sqrt-gl nm-ackley-gl nm-rosenbrock-gl nm-dixon-price-gl nm-st-gl nm-schwefel-gl nm-trid-gl

spiral-%-std: %.o spiral.o nelder_mead.o main-spiral.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

spiral-std: spiral-levy-std spiral-easom-std spiral-michalewicz-std spiral-sqrt-std spiral-rosenbrock-std spiral-box-std spiral-rastrigin-std spiral-ackley-std spiral-dixon-price-std spiral-st-std spiral-schwefel-std spiral-trid-std

spiral-%-gl: %.o spiral.o opengl.o soa-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

spiral-gl: spiral-levy-gl spiral-easom-gl spiral-michalewicz-gl spiral-sqrt-gl spiral-rosenbrock-gl spiral-rastrigin-gl spiral-ackley-gl spiral-dixon-price-gl spiral-st-gl spiral-schwefel-gl spiral-trid-gl

.PHONY: test test-16d test-8d test-3d test-2d test-1d ctags clean depclean

test: test-8d test-3d

test-16d: nogl
	@./nm-trid-std 3 0 16 1.0e-6 1000000 1.0 1 10 -100 100 >/dev/null
	@./spiral-trid-std 3 0 16 500 500 0 1 -100 100 >/dev/null
	@./spiral-trid-std 3 0 16 500 500 1 1 -100 100 >/dev/null
	@echo ""
	@./nm-ackley-std 3 0 16 1.0e-6 1000000 20.0 1 10 -20 20 >/dev/null
	@./spiral-ackley-std 3 0 16 500 500 0 1 -20 20 >/dev/null
	@./spiral-ackley-std 3 0 16 500 500 1 1 -20 20 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 3 0 16 1.0e-6 1000000 1.0 1 10 -20 20 >/dev/null
	@./spiral-rosenbrock-std 3 0 16 500 500 0 1 -20 20 >/dev/null
	@./spiral-rosenbrock-std 3 0 16 500 500 1 1 -20 20 >/dev/null
	@echo ""
	@./nm-dixon-price-std 3 0 16 1.0e-6 1000000 5.0 1 1000 -5 5 >/dev/null
	@./spiral-dixon-price-std 3 0 16 500 500 0 1 -5 5 >/dev/null
	@./spiral-dixon-price-std 3 0 16 500 500 1 1 -5 5 >/dev/null
	@echo ""
	@./nm-sqrt-std 3 0 16 1.0e-6 1000000 5.0 1 100 -20 20 >/dev/null
	@./spiral-sqrt-std 3 0 16 500 500 0 1 -20 20 >/dev/null
	@./spiral-sqrt-std 3 0 16 500 500 1 1 -20 20 >/dev/null
	@echo ""
	@./nm-michalewicz-std 3 0 16 1.0e-6 1000000 1.0 1 100 0 3.14 >/dev/null
	@./spiral-michalewicz-std 3 0 16 500 500 0 1 0 3.14 >/dev/null
	@./spiral-michalewicz-std 3 0 16 500 500 1 1 0 3.14 >/dev/null
	@echo ""
	@./nm-easom-std 3 0 16 1.0e-6 100000 5.0 1 10 -25 25 >/dev/null
	@./spiral-easom-std 3 0 16 500 500 0 1 -25 25 >/dev/null
	@./spiral-easom-std 3 0 16 500 500 1 1 -25 25 >/dev/null
	@echo ""

test-8d: nogl
	@./nm-trid-std 3 0 8 1.0e-6 100000 1.0 1 10 -25 25 >/dev/null
	@./spiral-trid-std 3 0 8 200 500 0 1 -10 10 >/dev/null
	@./spiral-trid-std 3 0 8 200 500 1 1 -10 10 >/dev/null
	@echo ""
	@./nm-ackley-std 3 0 8 1.0e-6 100000 5.0 1 10 -10 10 >/dev/null
	@./spiral-ackley-std 3 0 8 200 500 0 1 -10 10 >/dev/null
	@./spiral-ackley-std 3 0 8 200 500 1 1 -10 10 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 3 0 8 1.0e-6 100000 1.0 1 10 -10 10 >/dev/null
	@./spiral-rosenbrock-std 3 0 8 200 500 0 1 -10 10 >/dev/null
	@./spiral-rosenbrock-std 3 0 8 200 500 1 1 -10 10 >/dev/null
	@echo ""
	@./nm-st-std 3 0 8 1.0e-6 100000 10.0 1 50 -10 10 >/dev/null
	@./spiral-st-std 3 0 8 200 500 0 1 -10 10 >/dev/null
	@./spiral-st-std 3 0 8 200 500 1 1 -10 10 >/dev/null
	@echo ""
	@./nm-dixon-price-std 3 0 8 1.0e-6 100000 1.0 1 50 -5 5 >/dev/null
	@./spiral-dixon-price-std 3 0 8 200 500 0 1 -5 5 >/dev/null
	@./spiral-dixon-price-std 3 0 8 200 500 1 1 -5 5 >/dev/null
	@echo ""
	@./nm-levy-std 3 0 8 1.0e-6 100000 10.0 1 50 -10 10 >/dev/null
	@./spiral-levy-std 3 0 8 200 500 0 1 -10 10 >/dev/null
	@./spiral-levy-std 3 0 8 200 500 1 1 -10 10 >/dev/null
	@echo ""
	@./nm-sqrt-std 3 0 8 1.0e-6 100000 5.0 1 50 -10 10 >/dev/null
	@./spiral-sqrt-std 3 0 8 200 500 0 1 -10 10 >/dev/null
	@./spiral-sqrt-std 3 0 8 200 500 1 1 -10 10 >/dev/null
	@echo ""
	@./nm-michalewicz-std 3 0 8 1.0e-6 100000 1.0 1 50 0 3.14 >/dev/null
	@./spiral-michalewicz-std 3 0 8 200 500 0 1 0 3.14 >/dev/null
	@./spiral-michalewicz-std 3 0 8 200 500 1 1 0 3.14 >/dev/null
	@echo ""
	@./nm-easom-std 3 0 8 1.0e-6 100000 1.0 1 10 -15 15 >/dev/null
	@./spiral-easom-std 3 0 8 200 500 0 1 -15 15 >/dev/null
	@./spiral-easom-std 3 0 8 200 500 1 1 -15 15 >/dev/null
	@echo ""

test-3d: nogl
	@./nm-trid-std 3 0 3 1.0e-6 100000 1.0 1 1 -5 5 >/dev/null
	@./spiral-trid-std 3 0 3 30 100 0 0 -5 5 >/dev/null
	@./spiral-trid-std 3 0 3 30 100 1 0 -5 5 >/dev/null
	@echo ""
	@./nm-ackley-std 3 0 3 1.0e-6 100000 5.0 0 1 -10 10 >/dev/null
	@./spiral-ackley-std 3 0 3 30 100 0 0 -10 10 >/dev/null
	@./spiral-ackley-std 3 0 3 30 100 1 0 -10 10 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 3 0 3 1.0e-6 100000 1.0 0 1 -5 5 >/dev/null
	@./spiral-rosenbrock-std 3 0 3 30 100 0 0 -5 5 >/dev/null
	@./spiral-rosenbrock-std 3 0 3 30 100 1 0 -5 5 >/dev/null
	@echo ""
	@./nm-st-std 3 0 3 1.0e-6 100000 5.0 0 1 -5 5 >/dev/null
	@./spiral-st-std 3 0 3 30 100 0 0 -5 5 >/dev/null
	@./spiral-st-std 3 0 3 30 100 1 0 -5 5 >/dev/null
	@echo ""
	@./nm-dixon-price-std 3 0 3 1.0e-6 100000 1.0 0 1 -5 5 >/dev/null
	@./spiral-dixon-price-std 3 0 3 30 100 0 0 -5 5 >/dev/null
	@./spiral-dixon-price-std 3 0 3 30 100 1 0 -5 5 >/dev/null
	@echo ""
	@./nm-levy-std 3 0 3 1.0e-6 100000 5.0 0 1 -10 10 >/dev/null
	@./spiral-levy-std 3 0 3 30 100 0 0 -10 10 >/dev/null
	@./spiral-levy-std 3 0 3 30 100 1 0 -10 10 >/dev/null
	@echo ""
	@./nm-sqrt-std 3 0 3 1.0e-6 100000 5.0 0 1 -10 10 >/dev/null
	@./spiral-sqrt-std 3 0 3 30 100 0 0 -10 10 >/dev/null
	@./spiral-sqrt-std 3 0 3 30 100 1 0 -10 10 >/dev/null
	@echo ""
	@./nm-rastrigin-std 3 0 3 1.0e-6 1000000 5.0 0 1 -5 5 >/dev/null
	@./spiral-rastrigin-std 3 0 3 30 100 0 0 -5 5 >/dev/null
	@./spiral-rastrigin-std 3 0 3 30 100 1 0 -5 5 >/dev/null
	@echo ""
	@./nm-schwefel-std 3 0 3 1.0e-6 100000 100.0 0 1 -500 500 >/dev/null
	@./spiral-schwefel-std 3 0 3 30 100 0 0 -500.0 500.0 >/dev/null
	@./spiral-schwefel-std 3 0 3 30 100 1 0 -500.0 500.0 >/dev/null
	@echo ""
	@./nm-michalewicz-std 3 0 3 1.0e-6 100000 1.0 0 1 0 3.14 >/dev/null
	@./spiral-michalewicz-std 3 0 3 30 100 0 0 0 3.14 >/dev/null
	@./spiral-michalewicz-std 3 0 3 30 100 1 0 0 3.14 >/dev/null
	@echo ""
	@./nm-easom-std 3 0 3 1.0e-6 100000 1.0 0 1 -10 10 >/dev/null
	@./spiral-easom-std 3 0 3 30 100 0 0 -10 10 >/dev/null
	@./spiral-easom-std 3 0 3 30 100 1 0 -10 10 >/dev/null
	@echo ""

test-2d: nogl
	@./nm-trid-std 3 0 2 1.0e-6 10000 1.0 0 1 -5 5 >/dev/null
	@./nm-ackley-std 3 0 2 1.0e-6 10000 1.0 0 1 -5 5 >/dev/null
	@./nm-rosenbrock-std 3 0 2 1.0e-6 10000 1.0 0 1 -5 5 >/dev/null
	@./nm-st-std 3 0 2 1.0e-6 10000 10.0 0 1 -5 5 >/dev/null
	@./nm-dixon-price-std 3 0 2 1.0e-6 10000 1.0 0 1 -5 5 >/dev/null
	@./nm-levy-std 3 0 2 1.0e-6 10000 10.0 0 1 -10 10 >/dev/null
	@./nm-sqrt-std 3 0 2 1.0e-6 10000 5.0 0 1 -10 10 >/dev/null
	@./nm-rastrigin-std 3 0 2 1.0e-6 10000 10.0 0 1 -32.768 32.768 >/dev/null
	@./nm-schwefel-std 3 0 2 1.0e-6 10000 1.0 0 1 -500 500 >/dev/null
	@./nm-michalewicz-std 3 0 2 1.0e-6 10000 1.0 0 1 -3.14 3.14 >/dev/null
	@./nm-easom-std 3 0 2 1.0e-6 10000 1.0 0 1 -6.28 6.28 >/dev/null
	@./nm-box-std 3 0 2 1.0e-6 10000 1.0 0 1 0 4 >/dev/null
	@echo ""

test-1d: nogl
	@./nm-ackley-std 3 0 1 1.0e-6 10000 1.0 0 1 -10 10 >/dev/null
	@./nm-st-std 3 0 1 1.0e-6 10000 10.0 0 1 -10 10 >/dev/null
	@./nm-dixon-price-std 3 0 1 1.0e-6 10000 1.0 0 1 -10 10 >/dev/null
	@echo ""

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf *-std *-gl *.o

depclean: clean
	@rm -f *.d

-include *.d
