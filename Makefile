
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

all: nogl nm-gl mds-gl whale-gl spiral-gl

nogl: nm-std mds-std whale-std spiral-std

nm-%-std: %.o nelder_mead.o simplex.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

nm-std: nm-michalewicz-std nm-rastrigin-std nm-sqrt-std nm-ackley-std nm-rosenbrock-std nm-dixon-price-std nm-st-std nm-box-std nm-schwefel-std nm-trid-std

nm-%-gl: %.o nelder_mead.o simplex.o opengl.o simplex-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

nm-gl: nm-michalewicz-gl nm-rastrigin-gl nm-sqrt-gl nm-ackley-gl nm-rosenbrock-gl nm-dixon-price-gl nm-st-gl nm-schwefel-gl nm-trid-gl

mds-%-std: %.o multidirectional.o simplex.o main-mds.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

mds-std: mds-michalewicz-std mds-rastrigin-std mds-sqrt-std mds-ackley-std mds-rosenbrock-std mds-dixon-price-std mds-st-std mds-box-std mds-schwefel-std mds-trid-std

mds-%-gl: %.o multidirectional.o simplex.o opengl.o multi-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

mds-gl: mds-michalewicz-gl mds-rastrigin-gl mds-sqrt-gl mds-ackley-gl mds-rosenbrock-gl mds-dixon-price-gl mds-st-gl mds-schwefel-gl mds-trid-gl

whale-%-std: %.o whale.o nelder_mead.o simplex.o main-whale.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

whale-std: whale-michalewicz-std whale-sqrt-std whale-rosenbrock-std whale-box-std whale-rastrigin-std whale-ackley-std whale-dixon-price-std whale-st-std whale-schwefel-std whale-trid-std

whale-%-gl: %.o whale.o opengl.o woa-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

whale-gl: whale-michalewicz-gl whale-sqrt-gl whale-rosenbrock-gl whale-rastrigin-gl whale-ackley-gl whale-dixon-price-gl whale-st-gl whale-schwefel-gl whale-trid-gl

spiral-%-std: %.o spiral.o main-spiral.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

spiral-std: spiral-michalewicz-std spiral-sqrt-std spiral-rosenbrock-std spiral-box-std spiral-rastrigin-std spiral-ackley-std spiral-dixon-price-std spiral-st-std spiral-schwefel-std spiral-trid-std

spiral-%-gl: %.o spiral.o opengl.o soa-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

spiral-gl: spiral-michalewicz-gl spiral-sqrt-gl spiral-rosenbrock-gl spiral-rastrigin-gl spiral-ackley-gl spiral-dixon-price-gl spiral-st-gl spiral-schwefel-gl spiral-trid-gl

.PHONY: test test-16d test-8d test-3d test-2d test-1d ctags clean depclean

test: test-8d test-3d

test-16d: nogl
	@./nm-ackley-std 3 0 16 1.0e-6 1000000 5.0 1 1 -10 10 >/dev/null
	@./whale-ackley-std 3 0 16 500 500 0 -10 10 >/dev/null
	@echo ""
	@./nm-st-std 3 0 16 1.0e-6 1000000 10.0 1 1 -5 5 >/dev/null
	@./whale-st-std 3 0 16 500 500 0 -5 5 >/dev/null
	@echo ""
	@./nm-dixon-price-std 3 0 16 1.0e-6 1000000 1.0 1 1 -10 10 >/dev/null
	@./whale-dixon-price-std 3 0 16 500 500 0 -10 10 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 3 0 16 1.0e-6 1000000 1.0 1 1 -5 5 >/dev/null
	@./whale-rosenbrock-std 3 0 16 500 500 0 -5 5 >/dev/null
	@echo ""
	@./nm-trid-std 3 0 16 1.0e-6 1000000 1.0 1 1 -100 100 >/dev/null
	@./whale-trid-std 3 0 16 500 500 0 -100 100 >/dev/null
	@echo ""
	@./nm-rastrigin-std 3 0 16 1.0e-6 1000000 10.0 1 1 -32.768 32.768 >/dev/null
	@./whale-rastrigin-std 3 0 16 500 500 0 -32.768 32.768 >/dev/null
	@echo ""
	@./nm-sqrt-std 3 0 16 1.0e-6 1000000 1.0 1 1 -5 5 >/dev/null
	@./whale-sqrt-std 3 0 16 500 500 0 -5 5 >/dev/null
	@echo ""
	@./nm-michalewicz-std 3 0 16 1.0e-6 1000000 1.0 0 1 -3.14 3.14 >/dev/null
	@./whale-michalewicz-std 3 0 16 500 500 0 -3.14 3.14 >/dev/null
	@echo ""

test-8d: nogl
	@./nm-ackley-std 3 0 8 1.0e-6 20000 3.0 1 10 -10 10 >/dev/null
	@./whale-ackley-std 3 0 8 200 200 1 -10 10 >/dev/null
	@echo ""
	@./nm-st-std 3 0 8 1.0e-6 20000 10.0 1 10 -5 5 >/dev/null
	@./whale-st-std 3 0 8 200 200 1 -5 5 >/dev/null
	@echo ""
	@./nm-dixon-price-std 3 0 8 1.0e-6 20000 1.0 1 10 -10 10 >/dev/null
	@./whale-dixon-price-std 3 0 8 200 200 1 -10 10 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 3 0 8 1.0e-6 20000 1.0 1 10 -5 5 >/dev/null
	@./whale-rosenbrock-std 3 0 8 200 200 1 -5 5 >/dev/null
	@echo ""
	@./nm-trid-std 3 0 8 1.0e-6 20000 1.0 1 10 -25 25 >/dev/null
	@./whale-trid-std 3 0 8 200 200 1 -25 25 >/dev/null
	@echo ""
	@./nm-rastrigin-std 3 0 8 1.0e-6 20000 10.0 1 10 -32.768 32.768 >/dev/null
	@./whale-rastrigin-std 3 0 8 200 200 1 -32.768 32.768 >/dev/null
	@echo ""
	@./nm-sqrt-std 3 0 8 1.0e-6 20000 1.0 1 10 -5 5 >/dev/null
	@./whale-sqrt-std 3 0 8 200 200 1 -5 5 >/dev/null
	@echo ""
	@./nm-michalewicz-std 3 0 8 1.0e-6 20000 1.0 0 10 -3.14 3.14 >/dev/null
	@./whale-michalewicz-std 3 0 8 200 200 1 -3.14 3.14 >/dev/null
	@echo ""

test-3d: nogl
	@./nm-ackley-std 3 0 3 1.0e-6 10000 1.0 0 1 -10 10 >/dev/null
	@./whale-ackley-std 3 0 3 30 100 0 -10 10 >/dev/null
	@echo ""
	@./nm-st-std 3 0 3 1.0e-6 10000 1.0 0 1 -5 5 >/dev/null
	@./whale-st-std 3 0 3 30 100 0 -5 5 >/dev/null
	@echo ""
	@./nm-dixon-price-std 3 0 3 1.0e-6 10000 1.0 0 1 -10 10 >/dev/null
	@./whale-dixon-price-std 3 0 3 30 100 0 -10 10 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 3 0 3 1.0e-6 10000 1.0 0 1 -5 5 >/dev/null
	@./whale-rosenbrock-std 3 0 3 30 100 0 -5 5 >/dev/null
	@echo ""
	@./nm-trid-std 3 0 3 1.0e-6 10000 1.0 1 1 -5 5 >/dev/null
	@./whale-trid-std 3 0 3 30 100 0 -5 5 >/dev/null
	@echo ""
	@./nm-rastrigin-std 3 0 3 1.0e-6 10000 10.0 0 1 -32.768 32.768 >/dev/null
	@./whale-rastrigin-std 3 0 3 30 100 0 -32.768 32.768 >/dev/null
	@echo ""
	@./nm-sqrt-std 3 0 3 1.0e-6 10000 1.0 0 1 -5 5 >/dev/null
	@./whale-sqrt-std 3 0 3 30 100 0 -5 5 >/dev/null
	@echo ""
	@./nm-michalewicz-std 3 0 3 1.0e-6 10000 1.0 0 1 -3.14 3.14 >/dev/null
	@./whale-michalewicz-std 3 0 3 30 100 0 -3.14 3.14 >/dev/null
	@echo ""

test-2d: nogl
	@./nm-ackley-std 3 0 2 1.0e-6 10000 1.0 0 1 -5 5 >/dev/null
	@./nm-st-std 3 0 2 1.0e-6 10000 10.0 0 1 -5 5 >/dev/null
	@./nm-dixon-price-std 3 0 2 1.0e-6 10000 1.0 0 1 -5 5 >/dev/null
	@./nm-rosenbrock-std 3 0 2 1.0e-6 10000 1.0 0 1 -5 5 >/dev/null
	@./nm-trid-std 3 0 2 1.0e-6 10000 1.0 0 1 -5 5 >/dev/null
	@./nm-box-std 3 0 2 1.0e-6 10000 1.0 0 1 0 4 >/dev/null
	@./nm-schwefel-std 3 0 2 1.0e-6 10000 1.0 0 1 -500 500 >/dev/null
	@./nm-michalewicz-std 3 0 2 1.0e-6 10000 1.0 0 1 -3.14 3.14 >/dev/null
	@echo ""

test-1d: nogl
	@./nm-ackley-std 3 0 1 1.0e-6 10000 1.0 0 1 -10 10 >/dev/null
	@./nm-st-std 3 0 1 1.0e-6 10000 10.0 0 1 -10 10 >/dev/null
	@./nm-dixon-price-std 3 0 1 1.0e-6 10000 1.0 0 1 -10 10 >/dev/null
	@echo ""

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf nm-* mds-* whale-* spiral-* *.o

depclean: clean
	@rm -f *.d

-include *.d
