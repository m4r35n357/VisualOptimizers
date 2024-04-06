# Simple Gradient-free optimizers

Currently three candidates:

* Nelder-Mead (using "dual" regular initial simplexes)
* Spiral Optimization (using "dual" rotations)
* Optimization by Cut (NEW)

Presented for application in local or global optimization in up to 16 dimensions.
Additionally there is a "random" optimizer (with no OpenGL visualization) to use as a base-line.

## Pure c99 (plus optional 3D OpenGL visualization)

The programs can be built either with Clang or GCC.
All console programs are written to and depend _only_ on the c99 standard and library (strictly speaking, the WG14/N1256 _draft_ standard!).
External dependencies (OpenGL, FreeGLUT & GLEW) are only needed for the OpenGL plotters (*-gl).

All simulation floating point operations are executed in _long double_ precision.
This gives a choice of precision and performance on different platforms.

Platform | Long Double Implementation
----------|-----------
x86 / x86-64 | 80 bit hardware float
armhf | 64 bit hardware float
aarch64 | 128 bit _software_ float

These are converted to _float_ for use by OpenGL.

All scripts are in _Posix_ shell syntax.

## Quick Start

### Requirements - Debian/Ubuntu packages
```
sudo apt install bc git build-essential freeglut3-dev glew-utils libglew-dev
```

### Download
```
git clone https://github.com/m4r35n357/GradientFreeOptimizers/single-simplex-ic
cd single-simplex-ic
```

### c Build (Clang with debug by default)

```
make depclean
```
once only, unless you have edited any header files, then either of
```
make
make nogl
```
There should be NO errors or warnings.
The default build requires OpenGL development libraries, and produces the *-gl executables.
The nogl build is pure c99 and produces the *-std executables.
C tags are built automatically, which can make things more comfortable in some editors and IDEs.
Also, there is a pre-commit script for use with git, which rebuilds everything and runs some tests.
To run it:
```
./pre-commit
```
To install it:
```
cp pre-commit .git/hooks
```
Fast executables (using -O3 -fno-math-errno -flto) make a big difference with "bulk" running (e.g. multi-run Nelder-Mead, and particularly the stats script):
```
make CCC=gcc
make CCC=clang
```
See the Makefile for various additional "test" targets.
In particular:
```
make test-3d
```
shows basic program output (with stdout suppressed!) and can be used to get OpenGL commands by cut & paste - just change std to gl in the program names.
```
make test-8d
```
shows more output - random, multiple-run Nelder-Mead, spiral (both strategies), cut

# Usage

## Nelder-Mead

Nelder, John A.; R. Mead (1965). "A simplex method for function minimization". Computer Journal. 7 (4): 308–313. doi:10.1093/comjnl/7.4.308.

This code was originally based on a [project](https://github.com/matteotiziano/nelder-mead) by Matteo Maggioni in 2023.
It now uses "dual" regular simplexes (the best answer is selected), and has a "multi-run" mode for use as a global optimizer.

The implementation in this project is a little different from those in most popular/common solvers in that it uses a pair of _regular_ (dual) simplexes (the best answer is selected), and the user must specify an initial "scale".
Use the OpenGL visualizations to see how this works.
The vertex coordinates are generated using an algorithm which is described (open access) [here](https://link.springer.com/article/10.1007/s11590-022-01953-y).
The adaptive algorithm is described (open access) [here](https://www.researchgate.net/publication/225691623_Implementing_the_Nelder-Mead_simplex_algorithm_with_adaptive_parameters).

Parameter | Meaning
----------|-----------
1 | Display precision (3..36)
2 | Floating point format (0 for fixed, 1 for exponential)
3 | Number of dimensions
4 | Maximum error for convergence
5 | Maximum number of iterations
6 | Initial simplex scale
7 | Adaptive (0 for no, 1 for yes)
8 | Initialization (0 for explicit coordinates, 1 or more for number of random "runs") - ignored for GL

If Initialization = 0
Parameter | Meaning
----------|-----------
9+ | Coordinate list

If Initialization = 1
Parameter | Meaning
----------|-----------
9 | Lower limit
10 | Upper limit

Examples
```
./nm-ackley-std 3 0 3 1.0e-6 10000 1.0 0 1 -10 10 >/dev/null
./nm-ackley-std 3 0 3 1.0e-6 10000 1.0 0 100 -10 10 >/dev/null
./nm-ackley-gl 3 0 3 1.0e-6 10000 1.0 0 1 -10 10
```

## Spiral Optimization

Based on the algorithm described [here](https://en.wikipedia.org/wiki/Spiral_optimization_algorithm).
Note that current versions of the algorithm use a fixed rotation angle of PI/2; versions using variable theta are now obsolete.
The code supports both modes of operation described in that link: "Periodic Descent Direction Setting" and "Convergence Setting".

The implementation in this project uses a "dual" mode in which both "directions of rotation" of the spiral are used, and the best answer selected.
Use the OpenGL visualizations to see how this works.

Parameter | Meaning
----------|-----------
1 | Display precision (3..36)
2 | Floating point format (0 for fixed, 1 for exponential)
3 | Number of dimensions
4 | Number of search agents
5 | Number of iterations
6 | Algorithm mode (0 for "periodic descent", 1 for "convergence")
7 | Lower limit
8 | Upper limit

Examples
```
./spiral-ackley-std 3 0 3 50 100 0 -5 5
./spiral-ackley-std 3 0 3 50 100 1 -5 5
./spiral-ackley-gl 3 0 3 50 100 0 -5 5
```

##  Optimization by Cut

Algorithm described [here](https://arxiv.org/abs/2207.05953v1).

Use the OpenGL visualizations to see how this works.

Parameter | Meaning
----------|-----------
1 | Display precision (3..36)
2 | Floating point format (0 for fixed, 1 for exponential)
3 | Number of dimensions
4 | Number of search agents
5 | Number of iterations
6 | Lower limit
7 | Upper limit

Examples
```
./cut-ackley-std 3 0 3 50 100 -5 5
./cut-ackley-std 3 0 3 50 100 -5 5
./cut-ackley-gl 3 0 3 50 100 -5 5
```

## "solve-model" script

Run a model against all integrators, including random

Parameter | Meaning
----------|-----------
1 | Number of dimensions
2 | Number of search agents
3 | Number of iterations
4 | Number of Nelder-Mead runs
5 | Lower limit
6 | Upper limit

Example
```
./solve-model sphere 8 200 500 1 -10 10
```

To get a better idea of the variation in performance of spiral and cut integrators, use the "stats" script below.

## "stats" script

Runs spiral or cut algorithm multiple times agains a target value.
For each run, the output is green if the result is below the threshold, and red otherwise.

Parameter | Meaning
----------|-----------
1 | Number of runs
2 | Threshold
3+ | spiral or cut "std" command

Examples
```
./stats 100 -117 ./spiral-st-std 3 0 3 30 100 1 -5 5
./stats 100 -117 ./cut-st-std 3 0 3 30 100 -5 5
```
For 8D or higher, one of the faster "CCC=" make options above is recommended.

## "Global" Optimization

A common feature of "global" methods is an _exploration_ phase followed by a _refinement_ phase (in practice the transition is a gradual process).
Refinement is not the same as convergence; you get what you are given after a specified number of iterations!
Perhaps "settling" would be a better description.
In any case, this refinement stage makes it harder to jump out of a stubborn local minimum.

I have adapted the Nelder-Mead method to do a series of random runs, while keeping the best result. and accounting for total number of iterations and function evaluations.
This works surprisingly well!
The random exploration is not limited by any refinement process so global minima are always accessible, even if not actually reached within the set limits.

The [Dixon-Price function](https://www.sfu.ca/~ssurjano/dixonpr.html) is a very good example of a function with a stubborn local minimum (and _all_ minima at non-zero coordinates) that gets harder to escape as dimension increases.
To see the problem, try these commands, and then experiment with changing the number of agents, iterations etc.:
```
./stats 100 0.001 ./spiral-dixon-price-std 3 0 8 1000 1000 0 -10 10
./stats 100 0.001 ./spiral-dixon-price-std 3 0 8 1000 1000 1 -10 10
./nm-dixon-price-std 3 0 8 1.0e-6 100000 20.0 1 100 -10 10 >/dev/null
```
The [Styblinski-Tang function](https://www.sfu.ca/~ssurjano/stybtang.html) is also troublesome, but less "pathlogical".
```
./stats 100 -313.0 ./spiral-st-std 3 0 8 100 1000 0 -5 5
./stats 100 -313.0 ./spiral-st-std 3 0 8 100 1000 1 -5 5
./nm-st-std 3 0 8 1.0e-6 100000 10.0 1 100 -5 5 >/dev/null
```
Here are some 16-D starting points:
```
./nm-dixon-price-std 3 0 16 1.0e-6 1000000 10.0 1 1000 -10 10 >/dev/null
```
```
./nm-st-std 3 0 16 1.0e-6 1000000 10.0 1 1000 -5 5 >/dev/null
```
Of course this is comparing apples to oranges, with just two functions, and the results are not always clear-cut (and vary with each run), but total iterations and function evaluations are shown explicitly in each case.
Do your own experiments!

## OpenGL Visualizations

How to get a list of command examples:
```
grep -- '-std 3 0 3 ' Makefile | sed 's/^.*@//' | sed 's/-std/-gl/' | sed 's/ >.*$//' | sort
```

## OpenGL Keyboard Controls

The global minimum is no longer displayed by default, use the 'm' key to show it at any time.
For a more immersive "black-box" exerience, leave it until the end!

Key | Action
----------|-----------
UP arrow | increase view elevation
DOWN arrow | decrease view elevation
LEFT arrow | increase view azimuth
RIGHT arrow | decrease view azimuth
HOME | zoom in (fine)
END | zoom out (fine)
PAGE UP | zoom in (coarse)
PAGE DOWN | zoom out (coarse)
'C' 'c' | show/hide centroid (for Nelder-Mead only)
'M' 'm' | show/hide global minimum
'G' 'g' | increase ball size
'B' 'b' | decrease ball size
'S' 's' | iterate
'F' 'f' | toggle fullscreen mode
'H' 'h' | toggle OSD text
'Q' 'q' ESC | exit
