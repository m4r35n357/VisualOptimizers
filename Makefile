
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

nm-std: nm-sphere-std nm-levy-std nm-easom-std nm-michalewicz-std nm-rastrigin-std nm-treacle-std nm-ackley-std nm-rosenbrock-std nm-dixon-price-std nm-st-std nm-schwefel-std nm-trid-std

nm-%-gl: %.o nelder_mead.o opengl.o base.o simplex-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

nm-gl: nm-sphere-gl nm-levy-gl nm-easom-gl nm-michalewicz-gl nm-rastrigin-gl nm-treacle-gl nm-ackley-gl nm-rosenbrock-gl nm-dixon-price-gl nm-st-gl nm-schwefel-gl nm-trid-gl

# Optimization by cut
cut-%-std: %.o particles.o base.o main-particle.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

cut-std: cut-sphere-std cut-levy-std cut-easom-std cut-michalewicz-std cut-treacle-std cut-rosenbrock-std cut-rastrigin-std cut-ackley-std cut-dixon-price-std cut-st-std cut-schwefel-std cut-trid-std

cut-%-gl: %.o particles.o opengl.o base.o poa-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

cut-gl: cut-sphere-gl cut-levy-gl cut-easom-gl cut-michalewicz-gl cut-treacle-gl cut-rosenbrock-gl cut-rastrigin-gl cut-ackley-gl cut-dixon-price-gl cut-st-gl cut-schwefel-gl cut-trid-gl

.PHONY: test-multi-16-64 test-multi-3-16 test-64d test-32d test-16d test-8d test-3d ctags clean depclean

test-multi-16-64: nogl
	@./stats 10 0.001 ./nm-sphere-std 1 fixed 16 1.0e-6 100000 10.0 adaptive random 0 20 >/dev/null
	@./stats 10 0.001 ./nm-sphere-std 1 fixed 32 1.0e-6 100000 20.0 adaptive random 0 40 >/dev/null
	@./stats 10 0.001 ./nm-sphere-std 1 fixed 64 1.0e-6 100000 40.0 adaptive random 0 80 >/dev/null
	@echo ""
	@./stats 10 -799.0 ./nm-trid-std 1 fixed 16 1.0e-6 100000 50.0 adaptive random 0 100 >/dev/null
	@./stats 10 -5951.0 ./nm-trid-std 1 fixed 32 1.0e-6 100000 150.0 adaptive random 0 300 >/dev/null
	@./stats 10 -45695.0 ./nm-trid-std 1 fixed 64 1.0e-6 1000000 600.0 adaptive random 0 1200 >/dev/null
	@echo ""
	@./stats 10 0.001 ./nm-rosenbrock-std 1 fixed 16 1.0e-6 200000 10.0 adaptive random 0 20 >/dev/null
	@./stats 10 0.001 ./nm-rosenbrock-std 1 fixed 32 1.0e-6 200000 20.0 adaptive random 0 40 >/dev/null
	@./stats 10 0.001 ./nm-rosenbrock-std 1 fixed 64 1.0e-6 2000000 40.0 adaptive random 0 80 >/dev/null
	@echo ""
	@./stats 10 -0.999 ./nm-easom-std 1 fixed 16 1.0e-6 100000 10.0 adaptive random 0 20 >/dev/null
	@./stats 10 -0.999 ./nm-easom-std 1 fixed 32 1.0e-6 100000 20.0 adaptive random 0 40 >/dev/null
	@./stats 10 -0.999 ./nm-easom-std 1 fixed 64 1.0e-6 100000 40.0 adaptive random 0 80 >/dev/null
	@echo ""
	@./stats 10 0.126 ./nm-treacle-std 1 fixed 16 1.0e-6 100000 10.0 adaptive random 0 20 >/dev/null
	@./stats 10 0.179 ./nm-treacle-std 1 fixed 32 1.0e-6 100000 20.0 adaptive random 0 40 >/dev/null
	@./stats 10 0.253 ./nm-treacle-std 1 fixed 64 1.0e-6 1000000 40.0 adaptive random 0 80 >/dev/null
	@echo ""

test-multi-3-16: nogl
	@./multi-stats 100 0.001 sphere 3 27 100 0 5
	@./multi-stats 100 0.001 sphere 8 256 100 0 10
	@./multi-stats 10 0.001 sphere 16 65536 100 0 20
	@echo ""
	@./multi-stats 100 -6.9 trid 3 27 100 0 10
	@./multi-stats 100 -111.9 trid 8 256 100 0 30
	@./multi-stats 10 -799.0 trid 16 65536 100 0 100
	@echo ""
	@./multi-stats 100 0.001 rosenbrock 3 27 100 0 5
	@./multi-stats 100 0.001 rosenbrock 8 256 100 0 10
	@./multi-stats 10 0.001 rosenbrock 16 65536 100 0 20
	@echo ""
	@./multi-stats 100 -0.999 easom 3 27 100 0 10
	@./multi-stats 100 -0.999 easom 8 256 100 0 15
	@./multi-stats 10 -0.999 easom 16 65536 100 0 25
	@echo ""
	@./multi-stats 100 0.055 treacle 3 27 100 0 5
	@./multi-stats 100 0.089 treacle 8 256 100 0 10
	@./multi-stats 10 0.126 treacle 16 65536 100 0 20
	@echo ""
	@./multi-stats 100 0.001 ackley 3 27 100 0 5
	@./multi-stats 100 0.001 ackley 8 256 100 0 10
	@./multi-stats 10 0.001 ackley 16 65536 100 0 20
	@echo ""
	@./multi-stats 100 0.001 levy 3 27 100 0 5
	@./multi-stats 100 0.001 levy 8 256 100 0 10
	@./multi-stats 10 0.001 levy 16 65536 100 0 20
	@echo ""

test-64d: nogl
	@./nm-sphere-std 1 fixed 64 1.0e-6 100000 40.0 adaptive random 0 80 >/dev/null
	@echo ""
	@./nm-trid-std 1 fixed 64 1.0e-6 1000000 600.0 adaptive random 0 1200 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 1 fixed 64 1.0e-6 2000000 40.0 adaptive random 0 80 >/dev/null
	@echo ""
	@./nm-treacle-std 1 fixed 64 1.0e-6 1000000 40.0 adaptive random 0 80 >/dev/null
	@echo ""

test-32d: nogl
	@./nm-sphere-std 1 fixed 32 1.0e-6 100000 20.0 adaptive random 0 40 >/dev/null
	@echo ""
	@./nm-trid-std 1 fixed 32 1.0e-6 100000 150.0 adaptive random 0 300 >/dev/null
	@echo ""
	@./nm-rosenbrock-std 1 fixed 32 1.0e-6 200000 20.0 adaptive random 0 40 >/dev/null
	@echo ""
	@./nm-easom-std 1 fixed 32 1.0e-6 100000 20.0 adaptive random 0 40 >/dev/null
	@echo ""
	@./nm-treacle-std 1 fixed 32 1.0e-6 100000 20.0 adaptive random 0 40 >/dev/null
	@echo ""

# 2^D search agents
test-16d: nogl
	@./solve-model sphere 16 65536 100 0 20
	@echo ""
	@./solve-model trid 16 65536 100 0 100
	@echo ""
	@./solve-model rosenbrock 16 65536 100 0 20
	@echo ""
	@./solve-model easom 16 65536 100 0 25
	@echo ""
	@./solve-model treacle 16 65536 100 0 20
	@echo ""
	@./solve-model ackley 16 65536 100 0 20
	@echo ""
	@./solve-model levy 16 65536 100 0 20
	@echo ""

# 2^D search agents
test-8d: nogl
	@./solve-model sphere 8 256 100 0 10
	@echo ""
	@./solve-model trid 8 256 100 0 30
	@echo ""
	@./solve-model rosenbrock 8 256 100 0 10
	@echo ""
	@./solve-model easom 8 256 100 0 15
	@echo ""
	@./solve-model treacle 8 256 100 0 10
	@echo ""
	@./solve-model ackley 8 256 100 0 10
	@echo ""
	@./solve-model levy 8 256 100 0 10
	@echo ""
	@./solve-model dixon-price 8 256 100 -5 5
	@echo ""
	@./solve-model st 8 256 100 -5 10
	@echo ""
	@./solve-model michalewicz 8 256 100 0 3.14
	@echo ""

# 3^D search agents
test-3d: nogl
	@./solve-model sphere 3 27 100 0 5
	@echo ""
	@./solve-model trid 3 27 100 0 10
	@echo ""
	@./solve-model rosenbrock 3 27 100 0 5
	@echo ""
	@./solve-model easom 3 27 100 0 10
	@echo ""
	@./solve-model treacle 3 27 100 0 5
	@echo ""
	@./solve-model ackley 3 27 100 0 5
	@echo ""
	@./solve-model levy 3 27 100 0 5
	@echo ""
	@./solve-model dixon-price 3 27 100 -5 5
	@echo ""
	@./solve-model st 3 27 100 -5 5
	@echo ""
	@./solve-model michalewicz 3 27 100 0 3.14
	@echo ""
	@./solve-model rastrigin 3 27 100 0 5
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
