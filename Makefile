
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

all: nm-std nm-gl

nm-%-std: %.o nelder_mead.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

nm-std: nm-ackley-std nm-hartmann3-std nm-hartmann6-std nm-rosenbrock-std nm-himmelblau-std nm-sphere-std nm-st-std nm-rastrigin-std

nm-%-gl: %.o nelder_mead.o opengl.o simplex-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

nm-gl: nm-ackley-gl nm-hartmann3-gl nm-hartmann6-gl nm-rosenbrock-gl nm-himmelblau-gl nm-sphere-gl nm-st-gl nm-rastrigin-gl

.PHONY: test ctags clean depclean

test: all
	@./run.sh -b nm-ackley-std -d 6 -n 0 -p -2.10,-3.04,4.50 
	@./run.sh -b nm-sphere-std -d 6 -n 0 -p -2.10,-3.04,4.50
	@./run.sh -b nm-st-std -d 6 -n 0 -p 0.0,0.0,0.0
	@./run.sh -b nm-hartmann3-std -d 6 -n 0 -p 0.5,0.5,0.5
	@./run.sh -b nm-hartmann6-std -d 6 -n 0 -p 0.5,0.5,0.5,0.5,0.5,0.5
	@./run.sh -b nm-rosenbrock-std -d 6 -n 0 -p 1.0,0.0,-1.0
	@./run.sh -b nm-rosenbrock-std -d 6 -n 0 -p 1.0,0.0
	@./run.sh -b nm-himmelblau-std -d 6 -n 0 -p 3.0,3.0
	@./run.sh -b nm-himmelblau-std -d 6 -n 0 -p 3.0,-3.0
	@./run.sh -b nm-himmelblau-std -d 6 -n 0 -p -3.0,3.0
	@./run.sh -b nm-himmelblau-std -d 6 -n 0 -p -3.0,-3.0

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf nm-* *.o

depclean: clean
	@rm -f *.d

-include *.d
