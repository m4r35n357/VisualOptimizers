
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

nm-std: nm-easom-std nm-rosenbrock-std nm-trid-std nm-bw-std nm-e3-std nm-e5-std

nm-%-gl: %.o nelder_mead.o opengl.o base.o simplex-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

nm-gl: nm-easom-gl nm-rosenbrock-gl nm-trid-gl nm-bw-gl nm-e3-gl nm-e5-gl

# Optimization by cut
cut-%-std: %.o particles.o base.o main-particle.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

cut-std: cut-easom-std cut-rosenbrock-std cut-trid-std cut-bw-std cut-e3-std cut-e5-std

cut-%-gl: %.o particles.o opengl.o base.o poa-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

cut-gl: cut-easom-gl cut-rosenbrock-gl cut-trid-gl cut-bw-gl cut-e3-gl cut-e5-gl

.PHONY: test-multi-16-64 test-multi-3-8 test-multi test-filters test-8d test-3d ctags clean depclean

test-multi-16-64: nogl
	@./stats 10   -799.0 ./nm-trid-std 1 fixed 16 1.0e-6  100000  256.0 adaptive random  -256  256 >/dev/null
	@./stats 10  -5951.0 ./nm-trid-std 1 fixed 32 1.0e-6  100000 1024.0 adaptive random -1024 1024 >/dev/null
	@./stats 10 -45695.0 ./nm-trid-std 1 fixed 64 1.0e-6 1000000 4096.0 adaptive random -4096 4096 >/dev/null
	@echo ""
	@./stats 10 0.001 ./nm-rosenbrock-std 1 fixed 16 1.0e-6  200000 2.048 adaptive random -2.048 2.048 >/dev/null
	@./stats 10 0.001 ./nm-rosenbrock-std 1 fixed 32 1.0e-6  200000 2.048 adaptive random -2.048 2.048 >/dev/null
	@./stats 10 0.001 ./nm-rosenbrock-std 1 fixed 64 1.0e-6 2000000 2.048 adaptive random -2.048 2.048 >/dev/null
	@echo ""
	@./stats 10 -0.999 ./nm-easom-std 1 fixed 16 1.0e-6 100000 12.5 adaptive random 0 25 >/dev/null
	@./stats 10 -0.999 ./nm-easom-std 1 fixed 32 1.0e-6 100000 12.5 adaptive random 0 25 >/dev/null
	@./stats 10 -0.999 ./nm-easom-std 1 fixed 64 1.0e-6 100000 12.5 adaptive random 0 25 >/dev/null
	@echo ""

test-multi-3-8: nogl
	@./multi-stats 100   -6.9 trid 3  3    27 100   -9   9
	@./multi-stats 100 -111.9 trid 3  8   256 100  -64  64
	@echo ""
	@./multi-stats 100 0.001 rosenbrock 3  3    27 100 -2.048 2.048
	@./multi-stats 100 0.001 rosenbrock 3  8   256 100 -2.048 2.048
	@echo ""
	@./multi-stats 100 -0.999 easom 3  3    27 100 0 25
	@./multi-stats 100 -0.999 easom 3  8   256 100 0 25
	@echo ""

test-multi: nogl
	@./stats 100   -6.9 ./nm-trid-std 12 fixed  3 1.0e-9  100000   9.0 non-adaptive random   -9   9 >/dev/null
	@./stats 100 -111.9 ./nm-trid-std 12 fixed  8 1.0e-9  100000  64.0     adaptive random  -64  64 >/dev/null
	@./stats 100 -799.0 ./nm-trid-std 12 fixed 16 1.0e-9 1000000 256.0     adaptive random -256 256 >/dev/null
	@echo ""
	@./stats 100 0.001 ./nm-rosenbrock-std 12 fixed  3 1.0e-9  200000 2.048 non-adaptive random -2.048 2.048 >/dev/null
	@./stats 100 0.001 ./nm-rosenbrock-std 12 fixed  8 1.0e-9  200000 2.048     adaptive random -2.048 2.048 >/dev/null
	@./stats 100 0.001 ./nm-rosenbrock-std 12 fixed 16 1.0e-9 2000000 2.048     adaptive random -2.048 2.048 >/dev/null
	@echo ""
	@./stats 100 -0.999 ./nm-easom-std 12 fixed  3 1.0e-9 100000 12.5 non-adaptive random 0 25 >/dev/null
	@./stats 100 -0.999 ./nm-easom-std 12 fixed  8 1.0e-9 100000 12.5     adaptive random 0 25 >/dev/null
	@./stats 100 -0.999 ./nm-easom-std 12 fixed 16 1.0e-9 100000 12.5     adaptive random 0 25 >/dev/null
	@echo ""
	@./stats 100 1.0e-12 ./nm-bw-std 12 fixed 1 1.0e-9 100000 2.5 non-adaptive random 0.0 5.0 >/dev/null
	@./stats 100 1.0e-12 ./nm-bw-std 12 fixed 2 1.0e-9 100000 2.5 non-adaptive random 0.0 5.0 >/dev/null
	@./stats 100 1.0e-12 ./nm-bw-std 12 fixed 3 1.0e-9 100000 2.5 non-adaptive random 0.0 5.0 >/dev/null
	@./stats 100 1.0e-12 ./nm-bw-std 12 fixed 4 1.0e-9 100000 2.5 non-adaptive random 0.0 5.0 >/dev/null
	@./stats 100 1.0e-12 ./nm-bw-std 12 fixed 5 1.0e-9 100000 2.5     adaptive random 0.0 5.0 >/dev/null
	@./stats 100 1.0e-12 ./nm-bw-std 12 fixed 6 1.0e-9 100000 2.5     adaptive random 0.0 5.0 >/dev/null
	@./stats 100 1.0e-12 ./nm-bw-std 12 fixed 7 1.0e-9 100000 2.5     adaptive random 0.0 5.0 >/dev/null
	@./stats 100 1.0e-12 ./nm-bw-std 12 fixed 8 1.0e-9 100000 2.5     adaptive random 0.0 5.0 >/dev/null
	@echo ""
	@./multi-stats 100 1.0e-12 e3 12 3  27 100 0.0 5.0
	@echo ""
	@./multi-stats 100 1.0e-12 e5 12 6 216 100 0.0 5.0
	@echo ""

test-filters: nogl
	@./solve-model bw 2 1   2 100 0.0 5.0
	@echo ""
	@./solve-model bw 2 2   8 100 0.0 5.0
	@echo ""
	@./solve-model bw 2 3  27 100 0.0 5.0
	@echo ""
	@./solve-model bw 2 4  64 100 0.0 5.0
	@echo ""
	@./solve-model bw 2 5 125 100 0.0 5.0
	@echo ""
	@./solve-model bw 2 6 216 100 0.0 5.0
	@echo ""
	@./solve-model bw 2 7 343 100 0.0 5.0
	@echo ""
	@./solve-model bw 2 8 512 100 0.0 5.0
	@echo ""
	@./solve-model e3 2 3  27 100 0.0 5.0
	@echo ""
	@./solve-model e5 2 6 216 100 0.0 5.0
	@echo ""

# 2^D search agents
test-8d: nogl
	@./solve-model trid 3 8 256 100 -64 64
	@echo ""
	@./solve-model rosenbrock 3 8 256 100 -2.048 2.048
	@echo ""
	@./solve-model easom 3 8 256 100 0 25
	@echo ""
	@./solve-model bw 3 8 256 100 0.0 5.0
	@echo ""

# 3^D search agents
test-3d: nogl
	@./solve-model trid 3 3 27 100 -9 9
	@echo ""
	@./solve-model rosenbrock 3 3 27 100 -2.048 2.048
	@echo ""
	@./solve-model easom 3 3 27 100 0 25
	@echo ""
	@./solve-model bw 3 3 27 100 0.0 5.0
	@echo ""
	@./solve-model e3 3 3 27 100 0.0 5.0
	@echo ""

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf *-std *-gl *.o

depclean: clean
	@rm -f *.d

-include *.d
