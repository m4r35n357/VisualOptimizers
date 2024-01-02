
CFLAGS=-std=c99 -O3 -fno-math-errno -flto
WARNINGS=-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Wredundant-decls -Wmissing-declarations
LIB_STD=-lm
LIB_GL=-lGLEW -lglut -lGLU -lGL


ifeq ($(CCC),clang)
  CC=/usr/bin/clang
else  # default
  CC=/usr/bin/gcc
  CFLAGS += -s
  WARNINGS += -Wunsuffixed-float-constants
endif

%.o: %.c
	$(CC) $(CFLAGS) -MT $@ -MMD -MP -c -o $@ $< $(WARNINGS)

all: nm-ackley nm-hartmann3 nm-hartmann6 nm-rosenbrock nm-himmelblau nm-beale nm-sphere simplex-gl

nm-%: %.o nelder_mead.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

simplex-gl: nelder_mead.o sphere.o opengl.o simplex-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

.PHONY: test ctags clean depclean

test: all
	@./run.sh -b nm-ackley -p -2.10,-3.04,4.50 
	@./run.sh -b nm-sphere -p -2.10,-3.04,4.50
	@./run.sh -b nm-hartmann3 -p 0.5,0.5,0.5
	@#./run.sh -b nm-hartmann6 -p 0.5,0.5,0.5,0.5,0.5,0.5
	@./run.sh -b nm-rosenbrock -p 1.0,0.0
	@./run.sh -b nm-rosenbrock -p -1.0,0.0
	@./run.sh -b nm-rosenbrock -p -1.0,1.0
	@./run.sh -b nm-himmelblau -p 3.0,3.0
	@./run.sh -b nm-himmelblau -p 3.0,-3.0
	@./run.sh -b nm-himmelblau -p -3.0,3.0
	@./run.sh -b nm-himmelblau -p -3.0,-3.0
	@./run.sh -b nm-beale -p 0.0,0.0

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf nm-* *.o

depclean: clean
	@rm -f *.d

-include *.d
