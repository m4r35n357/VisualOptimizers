
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

# Automatic dependency generation
%.o: %.c
	$(CC) $(CFLAGS) -MT $@ -MMD -MP -c -o $@ $< $(WARNINGS)

all: nogl nm-gl spiral-gl cut-gl

nogl: nm-std spiral-std cut-std rnd-std ctags

# Random optimizarion
rnd-%-std: %.o nelder_mead.o base.o main-random.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

rnd-std: rnd-sphere-std rnd-levy-std rnd-easom-std rnd-michalewicz-std rnd-rastrigin-std rnd-treacle-std rnd-ackley-std rnd-rosenbrock-std rnd-dixon-price-std rnd-st-std rnd-schwefel-std rnd-trid-std

# Nelder-Mead optimizarion
nm-%-std: %.o nelder_mead.o base.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

nm-std: nm-sphere-std nm-levy-std nm-easom-std nm-michalewicz-std nm-rastrigin-std nm-treacle-std nm-ackley-std nm-rosenbrock-std nm-dixon-price-std nm-st-std nm-schwefel-std nm-trid-std

nm-%-gl: %.o nelder_mead.o opengl.o base.o simplex-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

nm-gl: nm-sphere-gl nm-levy-gl nm-easom-gl nm-michalewicz-gl nm-rastrigin-gl nm-treacle-gl nm-ackley-gl nm-rosenbrock-gl nm-dixon-price-gl nm-st-gl nm-schwefel-gl nm-trid-gl

# Spiral optimizarion
spiral-%-std: %.o spiral.o base.o main-spiral.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

spiral-std: spiral-sphere-std spiral-levy-std spiral-easom-std spiral-michalewicz-std spiral-treacle-std spiral-rosenbrock-std spiral-rastrigin-std spiral-ackley-std spiral-dixon-price-std spiral-st-std spiral-schwefel-std spiral-trid-std

spiral-%-gl: %.o spiral.o opengl.o base.o soa-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

spiral-gl: spiral-sphere-gl spiral-levy-gl spiral-easom-gl spiral-michalewicz-gl spiral-treacle-gl spiral-rosenbrock-gl spiral-rastrigin-gl spiral-ackley-gl spiral-dixon-price-gl spiral-st-gl spiral-schwefel-gl spiral-trid-gl

# Optimizarion by cut
cut-%-std: %.o cut.o base.o main-cut.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

cut-std: cut-sphere-std cut-levy-std cut-easom-std cut-michalewicz-std cut-treacle-std cut-rosenbrock-std cut-rastrigin-std cut-ackley-std cut-dixon-price-std cut-st-std cut-schwefel-std cut-trid-std

cut-%-gl: %.o cut.o opengl.o base.o coa-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

cut-gl: cut-sphere-gl cut-levy-gl cut-easom-gl cut-michalewicz-gl cut-treacle-gl cut-rosenbrock-gl cut-rastrigin-gl cut-ackley-gl cut-dixon-price-gl cut-st-gl cut-schwefel-gl cut-trid-gl

.PHONY: test-multi-16d test-multi-8d test-multi-3d test-64d test-32d test-16d test-8d test-3d test-2d ctags clean depclean

test-64d: nogl
	@./nm-sphere-std 1 0 64 1.0e-6 1000000 1.0 1 1 0 80 >/dev/null
	@echo ""
	@./nm-trid-std 1 0 64 1.0e-6 1000000 1.0 1 1 0 1056 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 1 0 64 1.0e-6 10000000 40.0 1 1 0 80 >/dev/null
	@echo ""

test-32d: nogl
	@./nm-sphere-std 1 0 32 1.0e-6 1000000 1.0 1 10 -30 30 >/dev/null
	@echo ""
	@./nm-trid-std 1 0 32 1.0e-6 1000000 1.0 1 10 -200 200 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 1 0 32 1.0e-6 1000000 1.0 1 10 -30 30 >/dev/null
	@echo ""
	@./nm-treacle-std 1 0 32 1.0e-6 1000000 30.0 1 100 -30 30 >/dev/null
	@echo ""
	@./nm-easom-std 1 0 32 1.0e-6 1000000 25.0 1 10 -10 40 >/dev/null
	@echo ""
	@./nm-dixon-price-std 1 0 32 1.0e-6 1000000 5.0 1 1000 -10 10 >/dev/null

# 2^16 search agents
test-multi-16d: nogl
	@./multi-stats 10 0.001 sphere 16 65536 100 -20 20
	@echo ""
	@./multi-stats 10 -799.0 trid 16 65536 100 -100 100
	@echo ""
	@./multi-stats 10 -0.999 easom 16 65536 100 -25 25
	@echo ""
	@./multi-stats 10 0.001 rosenbrock 16 65536 100 -20 20
	@echo ""
	@./multi-stats 10 0.01 treacle 16 65536 100 -20 20
	@echo ""
	@./multi-stats 10 -626.0 st 16 65536 100 -5 20
	@echo ""
	@./multi-stats 10 0.001 dixon-price 16 65536 100 -10 10
	@echo ""
	@./multi-stats 10 -14.5 michalewicz 16 65536 100 0 3.14
	@echo ""

# 2^16 search agents
test-16d: nogl
	@./solve-model sphere 16 65536 100 -20 20
	@echo ""
	@./solve-model trid 16 65536 100 -100 100
	@echo ""
	@./solve-model easom 16 65536 100 -25 25
	@echo ""
	@./solve-model rosenbrock 16 65536 100 -20 20
	@echo ""
	@./solve-model treacle 16 65536 100 -20 20
	@echo ""
	@./solve-model st 16 65536 100 -5 20
	@echo ""
	@./solve-model dixon-price 16 65536 100 -10 10
	@echo ""
	@./solve-model michalewicz 16 65536 100 0 3.14
	@echo ""

# 2^8 search agents
test-multi-8d: nogl
	@./multi-stats 100 0.001 sphere 8 256 1000 -10 10
	@echo ""
	@./multi-stats 100 -111.9 trid 8 256 1000 -30 30
	@echo ""
	@./multi-stats 100 -0.999 easom 8 256 1000 -15 15
	@echo ""
	@./multi-stats 100 0.001 rosenbrock 8 256 1000 -10 10
	@echo ""
	@./multi-stats 100 0.01 treacle 8 256 1000 -10 10
	@echo ""
	@./multi-stats 100 -313.0 st 8 256 1000 -5 10
	@echo ""
	@./multi-stats 100 0.001 dixon-price 8 256 1000 -10 10
	@echo ""
	@./multi-stats 100 0.001 levy 8 256 1000 -10 10
	@echo ""
	@./multi-stats 100 -7.5 michalewicz 8 256 1000 0 3.14
	@echo ""

# 2^8 search agents
test-8d: nogl
	@./solve-model sphere 8 256 1000 -10 10
	@echo ""
	@./solve-model trid 8 256 1000 -30 30
	@echo ""
	@./solve-model easom 8 256 1000 -15 15
	@echo ""
	@./solve-model rosenbrock 8 256 1000 -10 10
	@echo ""
	@./solve-model treacle 8 256 1000 -20 20
	@echo ""
	@./solve-model st 8 256 1000 -5 10
	@echo ""
	@./solve-model dixon-price 8 256 1000 -10 10
	@echo ""
	@./solve-model levy 8 256 1000 -10 10
	@echo ""
	@./solve-model michalewicz 8 256 1000 0 3.14
	@echo ""

# 3^3 search agents
test-multi-3d: nogl
	@./multi-stats 100 0.001 sphere 3 27 100 -10 10
	@echo ""
	@./multi-stats 100 -6.9 trid 3 27 100 -10 10
	@echo ""
	@./multi-stats 100 -0.999 easom 3 27 100 -10 10
	@echo ""
	@./multi-stats 100 0.001 rosenbrock 3 27 100 -5 5
	@echo ""
	@./multi-stats 100 0.01 treacle 3 27 100 -10 10
	@echo ""
	@./multi-stats 100 -117.0 st 3 27 100 -5 5
	@echo ""
	@./multi-stats 100 0.001 dixon-price 3 27 100 -10 10
	@echo ""
	@./multi-stats 100 0.001 levy 3 27 100 -10 10
	@echo ""
	@./multi-stats 100 -2.8 michalewicz 3 27 100 0 3.14
	@echo ""
	@./multi-stats 100 0.001 rastrigin 3 27 100 -5 5
	@echo ""
	@./multi-stats 100 0.001 schwefel 3 27 100 -500 500
	@echo ""

# 3^3 search agents
test-3d: nogl
	@./solve-model sphere 3 27 100 -10 10
	@echo ""
	@./solve-model trid 3 27 100 -10 10
	@echo ""
	@./solve-model easom 3 27 100 -10 10
	@echo ""
	@./solve-model rosenbrock 3 27 100 -5 5
	@echo ""
	@./solve-model treacle 3 27 100 -10 10
	@echo ""
	@./solve-model st 3 27 100 -5 5
	@echo ""
	@./solve-model dixon-price 3 27 100 -10 10
	@echo ""
	@./solve-model levy 3 27 100 -10 10
	@echo ""
	@./solve-model michalewicz 3 27 100 0 3.14
	@echo ""
	@./solve-model rastrigin 3 27 100 -5 5
	@echo ""
	@./solve-model schwefel 3 27 100 -500 500
	@echo ""

# 3^2 search agents
test-2d: nogl
	@./solve-model sphere 2 9 100 -10 10
	@echo ""
	@./solve-model trid 2 9 100 -10 10
	@echo ""
	@./solve-model easom 2 9 100 -10 10
	@echo ""
	@./solve-model rosenbrock 2 9 100 -5 5
	@echo ""
	@./solve-model treacle 2 9 100 -10 10
	@echo ""
	@./solve-model st 2 9 100 -5 5
	@echo ""
	@./solve-model dixon-price 2 9 100 -10 10
	@echo ""
	@./solve-model levy 2 9 100 -10 10
	@echo ""
	@./solve-model michalewicz 2 9 100 0 3.14
	@echo ""
	@./solve-model ackley 2 9 100 -5 5
	@echo ""
	@./solve-model rastrigin 2 9 100 -5 5
	@echo ""
	@./solve-model schwefel 2 9 100 -500 500
	@echo ""

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf *-std *-gl *.o

depclean: clean
	@rm -f *.d

-include *.d
