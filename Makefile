
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

all: nogl nm-gl cut-gl

nogl: nm-std cut-std ctags

# Nelder-Mead optimization
nm-%-std: %.o nelder_mead.o base.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

nm-std: nm-sphere-std nm-levy-std nm-easom-std nm-michalewicz-std nm-rastrigin-std nm-treacle-std nm-ackley-std nm-rosenbrock-std nm-dixon-price-std nm-st-std nm-schwefel-std nm-trid-std nm-lc-std

nm-%-gl: %.o nelder_mead.o opengl.o base.o simplex-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

nm-gl: nm-sphere-gl nm-levy-gl nm-easom-gl nm-michalewicz-gl nm-rastrigin-gl nm-treacle-gl nm-ackley-gl nm-rosenbrock-gl nm-dixon-price-gl nm-st-gl nm-schwefel-gl nm-trid-gl nm-lc-gl

# Optimization by cut
cut-%-std: %.o particles.o base.o main-particle.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

cut-std: cut-sphere-std cut-levy-std cut-easom-std cut-michalewicz-std cut-treacle-std cut-rosenbrock-std cut-rastrigin-std cut-ackley-std cut-dixon-price-std cut-st-std cut-schwefel-std cut-trid-std cut-lc-std

cut-%-gl: %.o particles.o opengl.o base.o poa-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

cut-gl: cut-sphere-gl cut-levy-gl cut-easom-gl cut-michalewicz-gl cut-treacle-gl cut-rosenbrock-gl cut-rastrigin-gl cut-ackley-gl cut-dixon-price-gl cut-st-gl cut-schwefel-gl cut-trid-gl cut-lc-gl

.PHONY: test-multi-16-64 test-multi-3-16 test-multi-logic test-multi-filters test-filters test-64d test-32d test-16d test-8d test-3d ctags clean depclean

test-multi-16-64: nogl
	@./stats 10 0.001 ./nm-sphere-std 1 fixed 16 1.0e-6 100000 10.0 adaptive random -10 10 >/dev/null
	@./stats 10 0.001 ./nm-sphere-std 1 fixed 32 1.0e-6 100000 10.0 adaptive random -10 10 >/dev/null
	@./stats 10 0.001 ./nm-sphere-std 1 fixed 64 1.0e-6 100000 10.0 adaptive random -10 10 >/dev/null
	@echo ""
	@./stats 10 -799.0 ./nm-trid-std 1 fixed 16 1.0e-6 100000 256.0 adaptive random -256 256 >/dev/null
	@./stats 10 -5951.0 ./nm-trid-std 1 fixed 32 1.0e-6 100000 1024.0 adaptive random -1024 1024 >/dev/null
	@./stats 10 -45695.0 ./nm-trid-std 1 fixed 64 1.0e-6 1000000 4096.0 adaptive random -4096 4096 >/dev/null
	@echo ""
	@./stats 10 0.001 ./nm-rosenbrock-std 1 fixed 16 1.0e-6 200000 2.048 adaptive random -2.048 2.048 >/dev/null
	@./stats 10 0.001 ./nm-rosenbrock-std 1 fixed 32 1.0e-6 200000 2.048 adaptive random -2.048 2.048 >/dev/null
	@./stats 10 0.001 ./nm-rosenbrock-std 1 fixed 64 1.0e-6 2000000 2.048 adaptive random -2.048 2.048 >/dev/null
	@echo ""
	@./stats 10 -0.999 ./nm-easom-std 1 fixed 16 1.0e-6 100000 12.5 adaptive random 0 25 >/dev/null
	@./stats 10 -0.999 ./nm-easom-std 1 fixed 32 1.0e-6 100000 12.5 adaptive random 0 25 >/dev/null
	@./stats 10 -0.999 ./nm-easom-std 1 fixed 64 1.0e-6 100000 12.5 adaptive random 0 25 >/dev/null
	@echo ""

test-multi-3-16: nogl
	@./multi-stats 100 0.001 sphere 3 27 100 -5.12 5.12
	@./multi-stats 100 0.001 sphere 8 256 100 -5.12 5.12
	@./multi-stats 10 0.001 sphere 16 65536 100 -5.12 5.12
	@echo ""
	@./multi-stats 100 -6.9 trid 3 27 100 -9 9
	@./multi-stats 100 -111.9 trid 8 256 100 -64 64
	@./multi-stats 10 -799.0 trid 16 65536 100 -256 256
	@echo ""
	@./multi-stats 100 0.001 rosenbrock 3 27 100 -2.048 2.048
	@./multi-stats 100 0.001 rosenbrock 8 256 100 -2.048 2.048
	@./multi-stats 10 0.001 rosenbrock 16 65536 100 -2.048 2.048
	@echo ""
	@./multi-stats 100 -0.999 easom 3 27 100 0 25
	@./multi-stats 100 -0.999 easom 8 256 100 0 25
	@./multi-stats 10 -0.999 easom 16 65536 100 0 25
	@echo ""
	@./multi-stats 100 0.055 treacle 3 27 100 -5.12 5.12
	@./multi-stats 100 0.089 treacle 8 256 100 -5.12 5.12
	@./multi-stats 10 0.126 treacle 16 65536 100 -5.12 5.12
	@echo ""
	@./multi-stats 100 0.001 ackley 3 27 100 -5.12 5.12
	@./multi-stats 100 0.001 ackley 8 256 100 -5.12 5.12
	@./multi-stats 10 0.001 ackley 16 65536 100 -5.12 5.12
	@echo ""
	@./multi-stats 100 0.001 levy 3 27 100 -10 10
	@./multi-stats 100 0.001 levy 8 256 100 -10 10
	@./multi-stats 10 0.001 levy 16 65536 100 -10 10
	@echo ""

test-multi-filters: nogl
	@./stats 1 0.000000001 ./nm-lc-std 9 fixed 1 1.0e-9 10000 1.5 non-adaptive point 3.0 >/dev/null
	@./stats 100 0.000000001 ./nm-lc-std 9 fixed 1 1.0e-9 10000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@echo ""
	@./stats 1 0.000000001 ./nm-lc-std 9 fixed 2 1.0e-9 10000 1.5 non-adaptive point 3.0 3.0 >/dev/null
	@./stats 100 0.000000001 ./nm-lc-std 9 fixed 2 1.0e-9 10000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@echo ""
	@./stats 1 0.000000001 ./nm-lc-std 9 fixed 3 1.0e-9 10000 1.5 non-adaptive point 3.0 3.0 3.0 >/dev/null
	@./stats 100 0.000000001 ./nm-lc-std 9 fixed 3 1.0e-9 10000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@echo ""
	@./stats 1 0.000000001 ./nm-lc-std 9 fixed 4 1.0e-9 10000 1.5 non-adaptive point 3.0 3.0 3.0 3.0 >/dev/null
	@./stats 100 0.000000001 ./nm-lc-std 9 fixed 4 1.0e-9 10000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@echo ""
	@./stats 1 0.000000001 ./nm-lc-std 9 fixed 5 1.0e-9 10000 1.5 non-adaptive point 3.0 3.0 3.0 3.0 3.0 >/dev/null
	@./stats 100 0.000000001 ./nm-lc-std 9 fixed 5 1.0e-9 10000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@echo ""
	@./stats 1 0.000000001 ./nm-lc-std 9 fixed 6 1.0e-9 10000 1.5 non-adaptive point 3.0 3.0 3.0 3.0 3.0 3.0 >/dev/null
	@./stats 100 0.000000001 ./nm-lc-std 9 fixed 6 1.0e-9 10000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@echo ""
	@./stats 1 0.000000001 ./nm-lc-std 9 fixed 7 1.0e-9 10000 1.5 non-adaptive point 3.0 3.0 3.0 3.0 3.0 3.0 3.0 >/dev/null
	@./stats 100 0.000000001 ./nm-lc-std 9 fixed 7 1.0e-9 10000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@echo ""
	@./stats 1 0.000000001 ./nm-lc-std 9 fixed 8 1.0e-9 10000 1.5 non-adaptive point 3.0 3.0 3.0 3.0 3.0 3.0 3.0 3.0 >/dev/null
	@./stats 100 0.000000001 ./nm-lc-std 9 fixed 8 1.0e-9 10000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@echo ""

test-filters: nogl
	@./nm-lc-std 6 fixed 8 1.0e-6 100000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@./nm-lc-std 6 fixed 7 1.0e-6 100000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@./nm-lc-std 6 fixed 6 1.0e-6 100000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@./nm-lc-std 6 fixed 5 1.0e-6 100000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@./nm-lc-std 6 fixed 4 1.0e-6 100000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@./nm-lc-std 6 fixed 3 1.0e-6 100000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@./nm-lc-std 6 fixed 2 1.0e-6 100000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@./nm-lc-std 6 fixed 1 1.0e-6 100000 1.5 non-adaptive random 0.0 3.0 >/dev/null
	@echo ""
	@./cut-lc-std 6 fixed 6 216 100 unclamped 0.0 3.0 >/dev/null
	@./cut-lc-std 6 fixed 5 125 100 unclamped 0.0 3.0 >/dev/null
	@./cut-lc-std 6 fixed 4 64 100 unclamped 0.0 3.0 >/dev/null
	@./cut-lc-std 6 fixed 3 27 100 unclamped 0.0 3.0 >/dev/null
	@./cut-lc-std 6 fixed 2 8 100 unclamped 0.0 3.0 >/dev/null
	@./cut-lc-std 6 fixed 1 2 100 unclamped 0.0 3.0 >/dev/null
	@echo ""

test-64d: nogl
	@./nm-sphere-std 1 fixed 64 1.0e-6 100000 5.12 adaptive random -5.12 5.12 >/dev/null
	@echo ""
	@./nm-trid-std 1 fixed 64 1.0e-6 1000000 4096.0 adaptive random -4096 4096 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 1 fixed 64 1.0e-6 2000000 2.048 adaptive random -2.048 2.048 >/dev/null
	@echo ""
	@./nm-easom-std 1 fixed 64 1.0e-6 100000 12.5 adaptive random 0 25 >/dev/null
	@echo ""

test-32d: nogl
	@./nm-sphere-std 1 fixed 32 1.0e-6 100000 5.12 adaptive random -5.12 5.12 >/dev/null
	@echo ""
	@./nm-trid-std 1 fixed 32 1.0e-6 100000 1024.0 adaptive random -1024 1024 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 1 fixed 32 1.0e-6 200000 2.048 adaptive random -2.048 2.048 >/dev/null
	@echo ""
	@./nm-easom-std 1 fixed 32 1.0e-6 100000 12.5 adaptive random 0 25 >/dev/null
	@echo ""

# 2^D search agents
test-16d: nogl
	@./solve-model sphere 16 65536 100 -5.12 5.12
	@echo ""
	@./solve-model trid 16 65536 100 -256 256
	@echo ""
	@./solve-model rosenbrock 16 65536 100 -2.048 2.048
	@echo ""
	@./solve-model easom 16 65536 100 0 25
	@echo ""
	@./solve-model treacle 16 65536 100 -5.12 5.12
	@echo ""
	@./solve-model ackley 16 65536 100 -5.12 5.12
	@echo ""
	@./solve-model levy 16 65536 100 -10 10
	@echo ""

# 2^D search agents
test-8d: nogl
	@./solve-model sphere 8 256 100 -5.12 5.12
	@echo ""
	@./solve-model trid 8 256 100 -64 64
	@echo ""
	@./solve-model rosenbrock 8 256 100 -2.048 2.048
	@echo ""
	@./solve-model easom 8 256 100 0 25
	@echo ""
	@./solve-model treacle 8 256 100 -5.12 5.12
	@echo ""
	@./solve-model ackley 8 256 100 -5.12 5.12
	@echo ""
	@./solve-model levy 8 256 100 -10 10
	@echo ""
	@./solve-model dixon-price 8 256 100 -10 10
	@echo ""
	@./solve-model st 8 256 100 -5 5
	@echo ""
	@./solve-model michalewicz 8 256 100 0 3.14
	@echo ""

# 3^D search agents
test-3d: nogl
	@./solve-model lc 3 27 100 0.0 3.0
	@echo ""
	@./solve-model sphere 3 27 100 -5.12 5.12
	@echo ""
	@./solve-model trid 3 27 100 -9 9
	@echo ""
	@./solve-model rosenbrock 3 27 100 -2.048 2.048
	@echo ""
	@./solve-model easom 3 27 100 0 25
	@echo ""
	@./solve-model treacle 3 27 100 -5.12 5.12
	@echo ""
	@./solve-model ackley 3 27 100 -5.12 5.12
	@echo ""
	@./solve-model levy 3 27 100 -10 10
	@echo ""
	@./solve-model dixon-price 3 27 100 -10 10
	@echo ""
	@./solve-model st 3 27 100 -5 5
	@echo ""
	@./solve-model michalewicz 3 27 100 0 3.14
	@echo ""
	@./solve-model rastrigin 3 27 100 -5.12 5.12
	@echo ""
	@./solve-model schwefel 3 27 100 0 500
	@echo ""

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf *-std *-gl *.o

depclean: clean
	@rm -f *.d

-include *.d
