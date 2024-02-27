
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

all: nm-std nm-gl mds-std mds-gl whale-std whale-gl spiral-std spiral-gl

nm-%-std: %.o nelder_mead.o simplex.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

nm-std: nm-rastrigin-std nm-sqrt-std nm-sphere-std nm-ackley-std nm-hartmann3-std nm-rosenbrock-std nm-dixon-price-std nm-himmelblau-std nm-st-std nm-box-std nm-schwefel-std nm-trid-std

nm-%-gl: %.o nelder_mead.o simplex.o opengl.o simplex-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

nm-gl: nm-rastrigin-gl nm-sqrt-gl nm-sphere-gl nm-ackley-gl nm-hartmann3-gl nm-rosenbrock-gl nm-dixon-price-gl nm-himmelblau-gl nm-st-gl nm-schwefel-gl nm-trid-gl

mds-%-std: %.o multidirectional.o simplex.o main-mds.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

mds-std: mds-rastrigin-std mds-sqrt-std mds-sphere-std mds-ackley-std mds-hartmann3-std mds-rosenbrock-std mds-dixon-price-std mds-himmelblau-std mds-st-std mds-box-std mds-schwefel-std mds-trid-std

mds-%-gl: %.o multidirectional.o simplex.o opengl.o multi-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

mds-gl: mds-rastrigin-gl mds-sqrt-gl mds-sphere-gl mds-ackley-gl mds-hartmann3-gl mds-rosenbrock-gl mds-dixon-price-gl mds-himmelblau-gl mds-st-gl mds-schwefel-gl mds-trid-gl

whale-%-std: %.o whale.o main-whale.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

whale-std: whale-sphere-std whale-sqrt-std whale-rosenbrock-std whale-box-std whale-rastrigin-std whale-ackley-std whale-dixon-price-std whale-st-std whale-schwefel-std whale-hartmann3-std whale-trid-std

whale-%-gl: %.o whale.o opengl.o woa-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

whale-gl: whale-sphere-gl whale-sqrt-gl whale-rosenbrock-gl whale-box-gl whale-rastrigin-gl whale-ackley-gl whale-dixon-price-gl whale-st-gl whale-schwefel-gl whale-hartmann3-gl whale-trid-gl

spiral-%-std: %.o spiral.o main-spiral.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

spiral-std: spiral-sphere-std spiral-sqrt-std spiral-rosenbrock-std spiral-box-std spiral-rastrigin-std spiral-ackley-std spiral-dixon-price-std spiral-st-std spiral-schwefel-std spiral-hartmann3-std spiral-trid-std

spiral-%-gl: %.o spiral.o opengl.o soa-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

spiral-gl: spiral-sphere-gl spiral-sqrt-gl spiral-rosenbrock-gl spiral-box-gl spiral-rastrigin-gl spiral-ackley-gl spiral-dixon-price-gl spiral-st-gl spiral-schwefel-gl spiral-hartmann3-gl spiral-trid-gl

.PHONY: test test-16d test-8d test-4d test-3d test-2d test-1d ctags clean depclean

test: test-4d test-3d test-2d test-1d

test-16d: all
	@./nm-ackley-std 3 0 16 1.0e-6 10000 5.0 1 -10 10 >/dev/null
	@./whale-ackley-std 3 0 16 200 500 -10 10 >/dev/null
	@echo ""
	@./nm-st-std 3 0 16 1.0e-6 10000 1.0 1 -5 5 >/dev/null
	@./whale-st-std 3 0 16 200 500 -5 5 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 3 0 16 1.0e-6 10000 1.0 1 -5 5 >/dev/null
	@./whale-rosenbrock-std 3 0 16 200 1000 -5 5 >/dev/null
	@echo ""
	@./nm-trid-std 3 0 16 1.0e-6 10000 1.0 1 -100 100 >/dev/null
	@./whale-trid-std 3 0 16 200 1000 -100 100 >/dev/null

test-8d: all
	@./nm-ackley-std 3 0 8 1.0e-6 10000 3.0 0 -10 10 >/dev/null
	@./whale-ackley-std 3 0 8 200 500 -10 10 >/dev/null
	@echo ""
	@./nm-st-std 3 0 8 1.0e-6 10000 2.0 1 -5 5 >/dev/null
	@./whale-st-std 3 0 8 200 500 -5 5 >/dev/null
	@echo ""
	@./nm-dixon-price-std 3 0 8 1.0e-6 10000 1.0 0 -10 10 >/dev/null
	@./whale-dixon-price-std 3 0 8 200 500 -10 10 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 3 0 8 1.0e-6 10000 1.0 1 -5 5 >/dev/null
	@./whale-rosenbrock-std 3 0 8 200 200 -5 5 >/dev/null
	@echo ""
	@./nm-rastrigin-std 3 0 8 1.0e-6 10000 10.0 0 -32.768 32.768 >/dev/null
	@./whale-rastrigin-std 3 0 8 200 500 -32.768 32.768 >/dev/null
	@echo ""
	@./nm-sqrt-std 3 0 8 1.0e-6 10000 1.0 1 -5 5 >/dev/null
	@./whale-sqrt-std 3 0 8 200 500 -5 5 >/dev/null

test-4d: all
	@./nm-ackley-std 3 0 4 1.0e-6 10000 1.0 0 -10 10 >/dev/null
	@./whale-ackley-std 3 0 4 100 200 -10 10 >/dev/null
	@echo ""
	@./nm-st-std 3 0 4 1.0e-6 10000 1.0 0 -5 5 >/dev/null
	@./whale-st-std 3 0 4 100 200 -5 5 >/dev/null
	@echo ""
	@./nm-dixon-price-std 3 0 4 1.0e-6 10000 1.0 0 -10 10 >/dev/null
	@./whale-dixon-price-std 3 0 4 100 200 -10 10 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 3 0 4 1.0e-6 100000 1.0 0 -5 5 >/dev/null
	@./whale-rosenbrock-std 3 0 4 100 200 -5 5 >/dev/null
	@echo ""
	@./nm-rastrigin-std 3 0 4 1.0e-6 10000 10.0 0 -32.768 32.768 >/dev/null
	@./whale-rastrigin-std 3 0 4 100 200 -32.768 32.768 >/dev/null
	@echo ""
	@./nm-sqrt-std 3 0 4 1.0e-6 10000 1.0 0 -5 5 >/dev/null
	@./whale-sqrt-std 3 0 4 100 200 -5 5 >/dev/null

test-3d: all
	@./nm-ackley-std 3 0 3 1.0e-6 10000 1.0 0 -10 10 >/dev/null
	@./whale-ackley-std 3 0 3 50 150 -10 10 >/dev/null
	@echo ""
	@./nm-st-std 3 0 3 1.0e-6 10000 1.0 0 -5 5 >/dev/null
	@./whale-st-std 3 0 3 50 150 -5 5 >/dev/null
	@echo ""
	@./nm-dixon-price-std 3 0 3 1.0e-6 10000 1.0 0 -10 10 >/dev/null
	@./whale-dixon-price-std 3 0 3 50 150 -10 10 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 3 0 3 1.0e-6 10000 1.0 0 -5 5 >/dev/null
	@./whale-rosenbrock-std 3 0 3 50 150 -5 5 >/dev/null
	@echo ""
	@./nm-rastrigin-std 3 0 3 1.0e-6 10000 10.0 0 -32.768 32.768 >/dev/null
	@./whale-rastrigin-std 3 0 3 50 150 -32.768 32.768 >/dev/null
	@echo ""
	@./nm-sqrt-std 3 0 3 1.0e-6 10000 1.0 0 -5 5 >/dev/null
	@./whale-sqrt-std 3 0 3 50 150 -5 5 >/dev/null
	@echo ""
	@./nm-schwefel-std 3 0 3 1.0e-6 10000 1.0 0 -500 500 >/dev/null
	@./whale-schwefel-std 3 0 3 50 150 -500.0 500.0 >/dev/null
	@echo ""
	@./nm-hartmann3-std 3 0 3 1.0e-6 10000 1.0 0 -5 5 >/dev/null
	@./whale-hartmann3-std 3 0 3 50 150 0.0 1.0 >/dev/null

test-2d: all
	@./nm-ackley-std 3 0 2 1.0e-6 10000 1.0 0 -5 5 >/dev/null
	@./nm-st-std 3 0 2 1.0e-6 10000 1.0 0 -5 5 >/dev/null
	@./nm-dixon-price-std 3 0 2 1.0e-6 10000 1.0 0 -5 5 >/dev/null
	@./nm-rosenbrock-std 3 0 2 1.0e-6 10000 1.0 0 -5 5 >/dev/null
	@./nm-himmelblau-std 3 0 2 1.0e-6 10000 1.0 0 -5 5 >/dev/null
	@./nm-box-std 3 0 2 1.0e-6 10000 1.0 0 0 4 >/dev/null
	@./whale-box-std 6 0 2 50 100 0.001 4.8 >/dev/null

test-1d: all
	@./nm-ackley-std 3 0 1 1.0e-6 10000 1.0 0 -10 10 >/dev/null
	@./nm-st-std 3 0 1 1.0e-6 10000 1.0 0 -10 10 >/dev/null
	@./nm-dixon-price-std 3 0 1 1.0e-6 10000 1.0 0 -10 10 >/dev/null

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf nm-* mds-* whale-* spiral-* *.o

depclean: clean
	@rm -f *.d

-include *.d
